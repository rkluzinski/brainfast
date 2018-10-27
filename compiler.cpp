#include "compiler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>

using namespace asmjit;

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
      //disabling scanloops
      else if (true && parser.isScanLoop()) {
	pointerOp(offset);
	offset = 0;
	scanLoop(parser, offset);
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
  assembler->mov(mem_size, x86::rdi);
  
  //calls calloc, rdi already holds the size argument
  assembler->mov(x86::rsi, 1);
  assembler->call(imm_ptr(calloc));

  //check if calloc returned null
  assembler->cmp(x86::rax, NULL);
  assembler->jne(MALLOC_OK);

  //if null, return 1
  assembler->mov(x86::rax, 1);
  assembler->ret();

  //stores address of the calloc'd memory
  assembler->bind(MALLOC_OK);
  assembler->mov(ptr, x86::rax);
  assembler->mov(mem_start, x86::rax);
}

//emits brainfuck assembly program footer
void BFCompilerX86::programFooter() {
  //free allocated memory
  assembler->mov(x86::rdi, mem_start);
  assembler->call(imm_ptr(free));

  //returns 0
  assembler->mov(x86::rax, 0);
  assembler->ret();
}

//emits pointer add/sub operation
void BFCompilerX86::pointerOp(addr_offset offset) {
  if (offset > 0)
    assembler->add(ptr, offset);
  else if (offset < 0)
    assembler->sub(ptr, -offset);
  //emit nothing if offset is 0
}

//emits add/sub immediate operation
void BFCompilerX86::arithmeticOp(addr_offset offset, imm_value imm) {
  if (imm != 0)
    assembler->add(x86::byte_ptr(ptr, offset), imm);
}

//emits write byte operation
void BFCompilerX86::byteOutOp(addr_offset offset) {
  assembler->movzx(x86::rdi, x86::byte_ptr(ptr, offset));
  assembler->call(imm_ptr(putchar));
}

//emits read byte operation
void BFCompilerX86::byteInOp(addr_offset offset) {
  assembler->call(imm_ptr(getchar));
  assembler->mov(x86::byte_ptr(ptr, offset), x86::al);
}

//emits loop start
void BFCompilerX86::loopStart() {
  Loop loop(assembler->newLabel(), assembler->newLabel());
  loopStack.push_back(loop);
  
  assembler->cmp(x86::byte_ptr(ptr), 0);
  assembler->je(loop.end);
  assembler->bind(loop.start);
}

//emits loop end
void BFCompilerX86::loopEnd() {
  Loop loop = loopStack.back();
  loopStack.pop_back();
  
  assembler->cmp(x86::byte_ptr(ptr), 0);
  assembler->jne(loop.start);
  assembler->bind(loop.end);
}

//emits a multiply accumulate operation
void BFCompilerX86::multiplyAccumulate(addr_offset src, addr_offset dst, imm_value imm) {
  assembler->mov(x86::rax, imm);
  assembler->mul(x86::byte_ptr(ptr, src));
  assembler->add(x86::byte_ptr(ptr, dst), x86::al);
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
  assembler->mov(x86::byte_ptr(ptr, offset), 0);
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
void *_memchr(void *ptr, int value, size_t size) {
  return memchr(ptr, value, size);
}

//memrchr is overloaded and cannot be called directly by asmjit
void *_memrchr(void *ptr, int value, size_t size) {
  return memrchr(ptr, value, size);
}

//emits optimzied code for scanning left
void BFCompilerX86::scanLeft(addr_offset offset) {
  assembler->mov(x86::rdi, mem_start);
  assembler->mov(x86::rsi, 0);
  assembler->mov(x86::rdx, ptr);
  assembler->sub(x86::rdx, mem_start);
  assembler->add(x86::rdx, 1);

  assembler->call(imm_ptr(_memrchr));
  assembler->mov(ptr, x86::rax);
}

//emits optimized code for scanning right
void BFCompilerX86::scanRight(addr_offset offset) {
  assembler->mov(x86::rdi, ptr);
  assembler->mov(x86::rsi, 0);
  assembler->mov(x86::rdx, mem_start);
  assembler->add(x86::rdx, mem_size);
  assembler->sub(x86::rdx, ptr);

  assembler->call(imm_ptr(_memchr));
  assembler->mov(ptr, x86::rax);
}

//emits optimized multiply loop code
void BFCompilerX86::multiplyLoop(BFParser &parser, addr_offset offset) {
  int ptr_delta = 0;
  bool done = false;

  //skip if loop counter zero
  Label isZero = assembler->newLabel();
  assembler->cmp(x86::byte_ptr(ptr, offset), 0);
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
  assembler->mov(x86::byte_ptr(ptr, offset), 0);
  assembler->bind(isZero);
}

