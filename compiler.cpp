#include "compiler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>

using namespace asmjit;
using namespace asmjit::x86;

//loop constructor
BFCompilerX86::Loop::Loop(Label _start, Label _end)
  : start(_start), end(_end) {}

BFCompilerX86::BFCompilerX86(X86Assembler *assm)
  : assembler(assm) {}

//compiles the file into x86 assembly
void BFCompilerX86::compile(const char *filename) {
  BFParser parser(filename);
  
  addr_offset offset = 0;
  programHeader();

  while (parser.hasNext()) {
    switch (parser.peek()) {
    case '>':
      offset++;
      parser.next();
      break;
      
    case '<':
      offset--;
      parser.next();
      break;
      
    case '+':
    case '-':   
      arithmeticOp(offset, arithmeticSum(parser));
      break;
      
    case '.':
      byteOutOp(offset);
      parser.next();
      break;
      
    case ',':
      byteInOp(offset);
      parser.next();
      break;
      
    case '[':
      if (parser.isClearLoop()) {
	clearLoop(offset);
	parser.consume(3);
      }
      else if (parser.isScanLoop()) {
	scanLoop(parser, offset);
	offset = 0;
      }
      else if (parser.isMultiplyLoop()) {
	multiplyLoop(parser, offset);
      }
      else {
	pointerOp(offset);
	offset = 0;
	loopStart();
	parser.next();
      }
      break;
      
    case ']':
      pointerOp(offset);
      offset = 0;
      loopEnd();
      parser.next();
      break;

    default:
      exit(1);
      break;
    }
  }

  programFooter();
}

//emits brainfuck assembly program header
void BFCompilerX86::programHeader() {
  Label MALLOC_OK = assembler->newLabel();
  assembler->mov(mem_size, rdi);
  
  //calls calloc, rdi already holds the size argument
  assembler->mov(rsi, 1);
  assembler->call(imm_ptr(calloc));

  //check if calloc returned null
  assembler->cmp(rax, 0);
  assembler->jne(MALLOC_OK);

  //if null, return 1
  assembler->mov(rax, 1);
  assembler->ret();

  assembler->bind(MALLOC_OK);

  //preserve r12-r14
  assembler->push(r12);
  assembler->push(r13);
  assembler->push(r14);

  //stores address of the calloc'd memory
  assembler->mov(data_ptr, rax);
  assembler->mov(mem_start, x86::rax);
}

//emits brainfuck assembly program footer
void BFCompilerX86::programFooter() {
  //free allocated memory
  assembler->mov(rdi, mem_start);
  assembler->call(imm_ptr(free));

  //preserve r12-r14
  assembler->pop(r14);
  assembler->pop(r13);
  assembler->pop(r12);

  //returns 0
  assembler->xor_(rax, rax);
  assembler->ret();
}

//emits pointer add/sub operation
void BFCompilerX86::pointerOp(addr_offset offset) {
  if (offset != 0)
    assembler->lea(data_ptr, byte_ptr(data_ptr, offset));
}

//emits add/sub immediate operation
void BFCompilerX86::arithmeticOp(addr_offset offset, imm_value imm) {
  if (imm != 0)
    assembler->add(byte_ptr(data_ptr, offset), imm);
}

//emits write byte operation
void BFCompilerX86::byteOutOp(addr_offset offset) {
  assembler->movzx(rdi, byte_ptr(data_ptr, offset));
  assembler->call(imm_ptr(putchar));
}

//emits read byte operation
void BFCompilerX86::byteInOp(addr_offset offset) {
  assembler->call(imm_ptr(getchar));
  assembler->mov(byte_ptr(data_ptr, offset), al);
}

//emits loop start
void BFCompilerX86::loopStart() {
  Loop loop(assembler->newLabel(), assembler->newLabel());
  loopStack.push_back(loop);
  
  assembler->cmp(byte_ptr(data_ptr), 0);
  assembler->je(loop.end);
  assembler->bind(loop.start);
}

//emits loop end
void BFCompilerX86::loopEnd() {
  Loop loop = loopStack.back();
  loopStack.pop_back();
  
  assembler->cmp(byte_ptr(data_ptr), 0);
  assembler->jne(loop.start);
  assembler->bind(loop.end);
}

//emits a multiply accumulate operation
void BFCompilerX86::multiplyAccumulate(addr_offset src, addr_offset dst, imm_value imm) {
  //dst = dst + src * imm
  assembler->mov(al, imm);
  assembler->mul(byte_ptr(data_ptr, src));
  assembler->add(byte_ptr(data_ptr, dst), al);
}

//sums sequential arithmetic operations
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
void BFCompilerX86::clearLoop(addr_offset offset) {
  assembler->mov(byte_ptr(data_ptr, offset), 0);
}

//emits optimized scan loop code
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

//memchr is overloaded and cannot be called directly by asmjit
void *_memchr(void *_ptr, int value, size_t size) {
  return memchr(_ptr, value, size);
}

//memrchr is overloaded and cannot be called directly by asmjit
void *_memrchr(void *_ptr, int value, size_t size) {
  return memrchr(_ptr, value, size);
}

//emits optimzied code for scanning left
void BFCompilerX86::scanLeft(addr_offset offset) {
  assembler->mov(rdi, mem_start);
  assembler->xor_(rsi, rsi);
  assembler->lea(rdx, byte_ptr(data_ptr, offset + 1));
  assembler->sub(rdx, mem_start);

  assembler->call(imm_ptr(_memrchr));
  assembler->mov(data_ptr, rax);
}

//emits optimized code for scanning right
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

