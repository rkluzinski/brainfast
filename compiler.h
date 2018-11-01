#ifndef BRAINFAST_COMPILER_H
#define BRAINFAST_COMPILER_H

#include <vector>
#include <asmjit/asmjit.h>
#include "parser.h"

#define MEMORY_SIZE 0xffff //65536

//types used by the compiler
typedef int16_t addr_offset;
typedef uint8_t imm_value;
typedef int (*BFProgram)(int);

class BFCompilerX86 {
 private:
  asmjit::X86Assembler *assembler;
  asmjit::X86Gp data_ptr = asmjit::x86::r12;
  asmjit::X86Gp mem_start = asmjit::x86::r13;
  asmjit::X86Gp mem_size = asmjit::x86::r14;

  //stores labels for the beginning and ending of a loop
  struct Loop {
    asmjit::Label start;
    asmjit::Label end;
    
    Loop(asmjit::Label _start, asmjit::Label _end);
  };
  std::vector<Loop> loopStack;

  //emits brainfuck assembly program header
  void programHeader();
  //emits brainfuck assembly program footer
  void programFooter();

  //emits pointer add/sub operation
  void pointerOp(addr_offset offset);
  //emits add/sub immediate operation
  void arithmeticOp(addr_offset offset, imm_value imm);
  //emits write byte operation
  void byteOutOp(addr_offset offset);
  //emits read byte operation
  void byteInOp(addr_offset offset);
  //emits loop start
  void loopStart();
  //emits loop end
  void loopEnd();

  //emits move immediate operation
  void moveImmOp(addr_offset offset, imm_value imm);
  //emits a multiply accumulate operation
  void multiplyAccumulate(addr_offset src, addr_offset dest, imm_value imm);

  //sums sequential arithmetic operations
  imm_value arithmeticSum(BFParser &p);

  //emits optimized clear loop code
  void clearLoop(addr_offset offset);
  //emits optimized scan loop code
  void scanLoop(BFParser &parser, addr_offset offset);
  //emits optimzied code for scanning left
  void scanLeft(addr_offset offset);
  //emits optimized code for scanning right
  void scanRight(addr_offset offset);
  //emits optimized multiply loop code
  void multiplyLoop(BFParser &parser, addr_offset offset);

 public:
  //constructor
  BFCompilerX86(asmjit::X86Assembler *assm);
  
  //compiles the file into x86 assembly
  void compile(const char *filename);
};

#endif
