#include "compiler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>

using namespace asmjit;
using namespace asmjit::x86;

//loop structure constructor
BFCompilerX86::Loop::Loop(Label _start, Label _end)
  : start(_start), end(_end) {}

//compiler constructor
BFCompilerX86::BFCompilerX86(X86Assembler *assm)
  : assembler(assm) {}

//compiles the brainfuck program into a function that
//uses the System V x86-64 ABI
//memory to allocate is passed through rdi
//returns 0 on success, 1 on failure to allocate memory
void BFCompilerX86::compile(const char *filename) {
  BFParser parser(filename);

  //track offset whenever possible
  addr_offset offset = 0;
  programHeader();

  while (parser.hasNext()) {
    switch (parser.peek()) {
    case '>': //increase offset
      offset++;
      parser.next();
      break;
      
    case '<': //decrease offset
      offset--;
      parser.next();
      break;
      
    case '+': //emit an arithmetic operation
    case '-':   
      arithmeticOp(offset, arithmeticSum(parser));
      break;
      
    case '.': //emit an output operation
      byteOutOp(offset);
      parser.next();
      break;
      
    case ',': //emit an input operation
      byteInOp(offset);
      parser.next();
      break;
      
    case '[':
      //output optimized clear loop
      if (parser.isClearLoop()) {
	clearLoop(offset);
	parser.consume(3);
      }
      //output optimized scanloop
      else if (parser.isScanLoop()) {
	scanLoop(parser, offset);
	offset = 0;
      }
      //output optimized multiply loop
      else if (parser.isMultiplyLoop()) {
	multiplyLoop(parser, offset);
      }
      //loop cannot be optimized by brainfast
      else {
	pointerOp(offset);
	offset = 0;
	loopStart();
	parser.next();
      }
      break;
      
    case ']': //emit end of loop
      pointerOp(offset);
      offset = 0;
      loopEnd();
      parser.next();
      break;

    default:
      break;
    }
  }

  programFooter();
}

//emits brainfuck assembly program header
//memory size is passed to the brainfuck program via rdi
//returns 1 on failure to allocate memory
//  MOV r14, rdi   stores the program size
//  MOV rsi, 1     size of each cell, fixed at one byte
//  CALL calloc
//  CMP rax, 0
//  JNE MALLOC_OK
//  MOV RAX, 1     return one on failure
//  RET
//  MALLOC_OK:
//  PUSH r12       preserve r12-r14
//  PUSH r13
//  PUSH r14
//  MOV r12, rax   move address of memory to r12
//  MOV r13, rax   stores the address of malloc'd mem
void BFCompilerX86::programHeader() {
  Label MALLOC_OK = assembler->newLabel();
  
  //calls calloc, rdi already holds the size argument
  assembler->mov(mem_size, rdi);
  assembler->mov(rsi, 1);
  assembler->call(imm_ptr(calloc));

  //check if calloc returned null
  assembler->cmp(rax, 0);
  assembler->jne(MALLOC_OK);

  //if null, return 1
  assembler->mov(rax, 1);
  assembler->ret();

  //jump destination
  assembler->bind(MALLOC_OK);

  //preserve r12-r14
  assembler->push(r12);
  assembler->push(r13);
  assembler->push(r14);

  //stores address of the calloc'd memory
  assembler->mov(data_ptr, rax);
  assembler->mov(mem_start, x86::rax);
}

//emits the brainfuck program footer
//frees memory, restores registers and returns 1
//  MOV rdi, r13
//  CALL free      frees malloc'd mem
//  POP r12        restores registers
//  POP r13
//  POP r14
//  XOR rax, rax   return 0
//  RET
void BFCompilerX86::programFooter() {
  //free allocated memory
  assembler->mov(rdi, mem_start);
  assembler->call(imm_ptr(free));

  //restore r12-r14
  assembler->pop(r14);
  assembler->pop(r13);
  assembler->pop(r12);

  //returns 0
  assembler->xor_(rax, rax);
  assembler->ret();
}

//increments/decrements the data pointer
//  LEA r12, [r12+offset]
void BFCompilerX86::pointerOp(addr_offset offset) {
  if (offset != 0)
    assembler->lea(data_ptr, byte_ptr(data_ptr, offset));
}

//adds an immediate value to the current cell
//  ADD BYTE PTR [r12+offset], imm
void BFCompilerX86::arithmeticOp(addr_offset offset, imm_value imm) {
  if (imm != 0)
    assembler->add(byte_ptr(data_ptr, offset), imm);
}

//outputs the value in the cell to stdout
//  MOVZX rdi, BYTE PTR [r12+offset]
//  CALL putchar
void BFCompilerX86::byteOutOp(addr_offset offset) {
  assembler->movzx(rdi, byte_ptr(data_ptr, offset));
  assembler->call(imm_ptr(putchar));
}

//read value from stdin to call
//  CALL getchar
//  MOV BYTE PTR [r12 + offset], al
void BFCompilerX86::byteInOp(addr_offset offset) {
  assembler->call(imm_ptr(getchar));
  assembler->mov(byte_ptr(data_ptr, offset), al);
}

//emits loop start code
//  CMP BYTE PTR [r12+offset], 0
//  JE LOOP_END
//  LOOP_START:
void BFCompilerX86::loopStart() {
  Loop loop(assembler->newLabel(), assembler->newLabel());
  loopStack.push_back(loop);
  
  assembler->cmp(byte_ptr(data_ptr), 0);
  assembler->je(loop.end);
  assembler->bind(loop.start);
}

//emits loop end code
//  CMP BYTE PTR [r12+offset], 0
//  JNE LOOP_START
//  LOOP_END:
void BFCompilerX86::loopEnd() {
  Loop loop = loopStack.back();
  loopStack.pop_back();
  
  assembler->cmp(byte_ptr(data_ptr), 0);
  assembler->jne(loop.start);
  assembler->bind(loop.end);
}

//emits a multiply accumulate operation
//computes: dst = dst + src * imm
//  MOV al, imm
//  MUL BYTE_PTR [r12+src]
//  ADD BYTE_PTR [r12+dst], al
void BFCompilerX86::multiplyAccumulate(addr_offset src, addr_offset dst, imm_value imm) {
  assembler->mov(al, imm);
  assembler->mul(byte_ptr(data_ptr, src));
  assembler->add(byte_ptr(data_ptr, dst), al);
}

//finds the sum of the next sequence of +, - operations
//value overflows/underflows at 255/0
//consumes tokens from the parser
imm_value BFCompilerX86::arithmeticSum(BFParser &p) {
  imm_value sum = 0;
  char next = p.peek();
  
  while (p.hasNext()) {
    if (next == '+') {
      sum++;
      p.next();
    }
    else if (next == '-') {
      sum--;
      p.next();
    }
    else {
      break;
    }

    next = p.peek();
  }

  return sum;
}

//emits optimized clear loop code
//  MOV BYTE PTR [r12+offset], 0
void BFCompilerX86::clearLoop(addr_offset offset) {
  assembler->mov(byte_ptr(data_ptr, offset), 0);
}

//emits optimized scan loop code
//calls one of scanleft or scanright
//consumes tokens from the parser
void BFCompilerX86::scanLoop(BFParser &parser, addr_offset offset) {
  parser.next();
  if (parser.next() == '>') {
    scanRight(offset);
  }
  else {
    scanLeft(offset);
  }
  parser.next();
}

//C++'s memchr and memrchr are overloaded and cannot be called directly
//by asmjit as the references cannot be resolved by the compiler
void *_memchr(void *_ptr, int value, size_t size) {
  return memchr(_ptr, value, size);
}

void *_memrchr(void *_ptr, int value, size_t size) {
  return memrchr(_ptr, value, size);
}

//emits optimzied code for scanning left
//computes: data_ptr = memrchr(mem_start, 0, data_ptr+offset+1-mem_start)
//  MOV rdi, r13
//  XOR rsi, rsi
//  LEA rdx, BYTE PTR [r12+offset+1]
//  SUB rdx, r13
//  CALL _memrchr
//  MOV r12, rax
void BFCompilerX86::scanLeft(addr_offset offset) {
  assembler->mov(rdi, mem_start);
  assembler->xor_(rsi, rsi);
  assembler->lea(rdx, byte_ptr(data_ptr, offset + 1));
  assembler->sub(rdx, mem_start);

  assembler->call(imm_ptr(_memrchr));
  assembler->mov(data_ptr, rax);
}

//emits optimized code for scanning right
//computes: data_ptr = memrchr(data_ptr+offset, 0, mem_start+mem_size-data_ptr-offset)
//  MOV rdi, r13
//  XOR rsi, rsi
//  MOV rdx, r13
//  ADD rdx, r14
//  SUB rdx, r12
//  SUB rdx, offset
//  CALL _memchr
//  MOV r12, rax
void BFCompilerX86::scanRight(addr_offset offset) {
  assembler->lea(rdi, byte_ptr(data_ptr, offset));
  assembler->xor_(rsi, rsi);
  assembler->mov(rdx, mem_start);
  assembler->add(rdx, mem_size);
  assembler->sub(rdx, data_ptr);
  assembler->sub(rdx, offset);

  assembler->call(imm_ptr(_memchr));
  assembler->mov(data_ptr, rax);
}

//emits optimized multiply loop code
//consumes tokens from the parser
//  CMP BYTE PTR [r12+offset], 0
//  JE IS_ZERO
//  multiply accumulate 1
//  mulitply accumualte 2
//  ...
//  multiply accumulate n
//  MOV BYTE PTR [r12+offset], 0
//  IS_ZERO:
void BFCompilerX86::multiplyLoop(BFParser &parser, addr_offset offset) {
  int ptr_delta = 0;
  bool done = false;

  //skip if loop counter zero
  Label isZero = assembler->newLabel();
  assembler->cmp(byte_ptr(data_ptr, offset), 0);
  assembler->je(isZero);

  parser.next();
  while (!done) {
    switch(parser.peek()) {
    case '>':
      ptr_delta++;
      parser.next();
      break;
    case '<':
      ptr_delta--;
      parser.next();
      break;
    case '+':
    case '-':
      //emit multiply accumulate op
      if (ptr_delta != 0) {
	imm_value imm = arithmeticSum(parser);
	multiplyAccumulate(offset, offset + ptr_delta, imm);
      }
      else {
	parser.next();
      }
      break;
      
    case ']':
      done = true;
      parser.next();
      break;
    }
  }

  //clear loop counter
  assembler->mov(byte_ptr(data_ptr, offset), 0);
  assembler->bind(isZero);
}

