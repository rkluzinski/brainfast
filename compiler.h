#ifndef BRAINFAST_COMPILER_H
#define BRAINFAST_COMPILER_H

#include <vector>
#include <asmjit/asmjit.h>
#include "parser.h"

#define MEMORY_SIZE 0xffff //65536

//types used by the compiler
typedef int16_t addr_offset;
typedef uint8_t imm_value;

class BFCompilerX86 {
 private:
  imm_value *memory;
  asmjit::X86Gp ptr = asmjit::x86::r12;
  asmjit::X86Assembler *assembler;

  //stores labels for the beginning and ending of a loop
  struct Loop {
    asmjit::Label start;
    asmjit::Label end;
    
    Loop(asmjit::Label _start, asmjit::Label _end);
  };
  std::vector<Loop> loopStack;

  //emits pointer add/sub operation
  void pointerOp(addr_offset offset);
  //emits add/sub immediate operation
  void arithmeticOp(addr_offset offset, imm_value imm);
  //emits write byte operation
  void byteOutOp(addr_offset offset);
  //emits read byte operation
  void byteInOp(addr_offset offset);
  //emits loop start
  void loopStart(addr_offset offset);
  //emits loop end
  void loopEnd(addr_offset offset);

  //emits move immediate operation
  void moveImmOp(addr_offset offset, imm_value imm);
  //emits a multiply accumulate operation
  void mulitpyAccumulateOp(addr_offset src, addr_offset dest, imm_value imm);

  //sums sequential arithmetic operations
  imm_value arithmeticSum(BFParser &p);

  //emits optimized clear loop code
  void clearLoop(addr_offset offset);
  //emits optimized scan loop code
  void scanLoop(addr_offset offset);
  //emits optimized multiply loop code
  void multiplyLoop(addr_offset offset);

 public:
  //constructor
  //creates the compiler parser and jit environment
  BFCompilerX86(asmjit::X86Assembler *assembler);
  ~BFCompilerX86();

  //compiles the file into x86 assembly
  void compile(const char *filename);
};

#endif
