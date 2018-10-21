#include "compiler.h"
#include <stdio.h>
#include <stdlib.h>

using namespace asmjit;

//loop constructor
BFCompilerX86::Loop::Loop(asmjit::Label _start, asmjit::Label _end)
  : start(_start), end(_end) {}

//constructor
//allocates the programs memory
BFCompilerX86::BFCompilerX86(X86Assembler *assm) {
  assembler = assm;
  memory = (imm_value*) malloc(MEMORY_SIZE * sizeof(imm_value));
  if (memory == NULL) {
    //throw an error
    exit(0);
  }
}

BFCompilerX86::~BFCompilerX86() {
  free(memory);
}

//compiles the file into x86 assembly
void BFCompilerX86::compile(const char *filename) {
  BFParser parser(filename);

  addr_offset offset = 0;
  addr_offset loop_offset = 0;
  std::vector<addr_offset> offset_stack;

  //assembly header
  assembler->mov(ptr, (intptr_t) memory);

  while (parser.hasNext()) {
    switch (parser.peek()) {
    case '>':
      offset++;
      loop_offset++;
      parser.next();
      break;
      
    case '<':
      offset--;
      loop_offset--;
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
      if (parser.isClearLoop())
	break;
      if (parser.isScanLoop())
	break;
      if (parser.isMultiplyLoop())
	break;
      
      offset_stack.push_back(loop_offset);
      loop_offset = 0;
      
      loopStart(offset);
      parser.next();
      break;
      
    case ']':
      pointerOp(loop_offset);
      offset -= loop_offset;
      
      loop_offset = offset_stack.back();
      offset_stack.pop_back();
      
      loopEnd(offset);
      parser.next();
      break;

    default:
      //throw error
      exit(0);
      break;
    }
  }

  //assembly footer
  assembler->ret();
}

//emits pointer add/sub operation
void BFCompilerX86::pointerOp(addr_offset offset) {
  if (offset > 0)
    assembler->add(ptr, offset);
  else if (offset < 0)
    assembler->sub(ptr, -offset);
}

//emits add/sub immediate operation
void BFCompilerX86::arithmeticOp(addr_offset offset, imm_value imm) {
  assembler->add(x86::byte_ptr(ptr, offset), imm);
}

//emits write byte operation
void BFCompilerX86::byteOutOp(addr_offset offset) {
  /*assembler->mov(x86::rax, 1);
  assembler->mov(x86::rdi, 1);
  assembler->lea(x86::rsi, x86::byte_ptr(ptr, offset));
  assembler->mov(x86::rdx, 1);
  assembler->syscall();*/
  assembler->movzx(x86::rdi, x86::byte_ptr(ptr, offset));
  assembler->call(imm_ptr(putchar));
}

//emits read byte operation
void BFCompilerX86::byteInOp(addr_offset offset) {
  /*assembler->mov(x86::rax, 0);
  assembler->mov(x86::rdi, 0);
  assembler->lea(x86::rsi, x86::byte_ptr(ptr, offset));
  assembler->mov(x86::rdx, 1);
  assembler->syscall();*/
  assembler->call(imm_ptr(getchar));
  assembler->mov(x86::byte_ptr(ptr, offset), x86::al);
}

//emits loop start
void BFCompilerX86::loopStart(addr_offset offset) {
  Loop loop(assembler->newLabel(), assembler->newLabel());
  loop_stack.push_back(loop);
  
  assembler->cmp(x86::byte_ptr(ptr, offset), 0);
  assembler->je(loop.end);
  assembler->bind(loop.start);
}

//emits loop end
void BFCompilerX86::loopEnd(addr_offset offset) {
  Loop loop = loop_stack.back();
  loop_stack.pop_back();
  
  assembler->cmp(x86::byte_ptr(ptr, offset), 0);
  assembler->jne(loop.start);
  assembler->bind(loop.end);
}

//emits a move immediate operation
void moveImmOp(addr_offset offset, imm_value imm);
//emits a multiply accumulate operation
void mulitpyAccumulateOp(addr_offset src, addr_offset dest, imm_value imm);

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
void clearLoop(addr_offset offset);
//emits optimized scan loop code
void scanLoop(addr_offset offset);
//emits optimized multiply loop code
void multiplyLoop(addr_offset offset);
