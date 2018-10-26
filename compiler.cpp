#include "compiler.h"
#include <stdio.h>
#include <stdlib.h>

using namespace asmjit;

//loop constructor
BFCompilerX86::Loop::Loop(asmjit::Label _start, asmjit::Label _end)
  : start(_start), end(_end) {}

//constructor
//allocates the programs memory
BFCompilerX86::BFCompilerX86() {
  //assembler = assm;
}

BFCompilerX86::~BFCompilerX86() {
  //release runtime
}

//compiles the file into x86 assembly
BFProgram BFCompilerX86::compile(const char *filename) {
  BFParser parser(filename);

  CodeHolder code;
  code.init(runtime.getCodeInfo());
  assembler = new X86Assembler(&code);
  
  addr_offset offset = 0;

  //assembly header
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
	//scanLoop(offset);
      }
      else if (parser.isMultiplyLoop()) {
	//multiplyLoop(offset);
      }
      else {
	pointerOp(offset);
	offset = 0;
	loopStart(0);
	parser.next();
      }
      break;
      
    case ']':
      pointerOp(offset);
      offset = 0;
      loopEnd(0);
      parser.next();
      break;

    default:
      //throw error
      exit(0);
      break;
    }
  }

  programFooter();
  delete assembler;

  BFProgram fn;
  Error error = runtime.add(&fn, &code);
  if (error)
    return NULL;

  return fn;
}

//emits brainfuck assembly program header
void BFCompilerX86::programHeader() {
  Label MALLOC_OK = assembler->newLabel();
  //calls calloc, rdi already holds the size argument
  assembler->mov(x86::rsi, 0x1);
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
  assembler->mov(malloc_addr, x86::rax);
}

//emits brainfuck assembly program footer
void BFCompilerX86::programFooter() {
  //free allocated memory
  assembler->mov(x86::rdi, malloc_addr);
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
void BFCompilerX86::loopStart(addr_offset offset) {
  Loop loop(assembler->newLabel(), assembler->newLabel());
  loopStack.push_back(loop);
  
  assembler->cmp(x86::byte_ptr(ptr), 0);
  assembler->je(loop.end);
  assembler->bind(loop.start);
}

//emits loop end
void BFCompilerX86::loopEnd(addr_offset offset) {
  Loop loop = loopStack.back();
  loopStack.pop_back();
  
  assembler->cmp(x86::byte_ptr(ptr), 0);
  assembler->jne(loop.start);
  assembler->bind(loop.end);
}

//emits a move immediate operation
void BFCompilerX86::moveImmOp(addr_offset offset, imm_value imm) {

}

//emits a multiply accumulate operation
void BFCompilerX86::mulitpyAccumulateOp(addr_offset src, addr_offset dest, imm_value imm) {

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
void BFCompilerX86::scanLoop(addr_offset offset) {

}

//emits optimized multiply loop code
void BFCompilerX86::multiplyLoop(addr_offset offset) {

}
