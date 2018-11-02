#ifndef BRAINFAST_COMPILER_H
#define BRAINFAST_COMPILER_H

#include <vector>
#include <asmjit/asmjit.h>
#include "parser.h"

//types used by the compiler
typedef int16_t addr_offset;
typedef uint8_t imm_value;
typedef int (*BFProgram)(int);

//BFCompilerX86 turns a brainfuck program into assembly code that
//can be called by a C/C++ program. The function adheres to the
//System V x86-64 ABI. The size of memory to allocated is passed in
//(via rdi) and 0 is return on success.

class BFCompilerX86 {
 private:
  //for assembling the code
  asmjit::X86Assembler *assembler;

  //variables used by the brainfuck program
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
  void programHeader();
  
  //emits the brainfuck program footer
  //frees memory, restores registers and returns 1
  //  MOV rdi, r13
  //  CALL free      frees malloc'd mem
  //  POP r12        restores registers
  //  POP r13
  //  POP r14
  //  XOR rax, rax   return 0
  //  RET
  void programFooter();

  //increments/decrements the data pointer
  //  LEA r12, [r12+offset]
  void pointerOp(addr_offset offset);
  
  //adds an immediate value to the current cell
  //  ADD BYTE PTR [r12+offset], imm
  void arithmeticOp(addr_offset offset, imm_value imm);
  
  //outputs the value in the cell to stdout
  //  MOVZX rdi, BYTE PTR [r12+offset]
  //  CALL putchar
  void byteOutOp(addr_offset offset);
  
  //read value from stdin to call
  //  CALL getchar
  //  MOV BYTE PTR [r12 + offset], al
  void byteInOp(addr_offset offset);
  
  //emits loop start code
  //  CMP BYTE PTR [r12+offset], 0
  //  JE LOOP_END
  //  LOOP_START:
  void loopStart();
  
  //emits loop end code
  //  CMP BYTE PTR [r12+offset], 0
  //  JNE LOOP_START
  //  LOOP_END:
  void loopEnd();
  
  //emits a multiply accumulate operation
  //computes: dst = dst + src * imm
  //  MOV al, imm
  //  MUL BYTE_PTR [r12+src]
  //  ADD BYTE_PTR [r12+dst], al
  void multiplyAccumulate(addr_offset src, addr_offset dest, imm_value imm);

  //finds the sum of the next sequence of +, - operations
  //value overflows/underflows at 255/0
  //consumes tokens from the parser
  imm_value arithmeticSum(BFParser &p);

  //emits optimized clear loop code
  //  MOV BYTE PTR [r12+offset], 0
  void clearLoop(addr_offset offset);
  
  //emits optimized scan loop code
  //calls one of scanleft or scanright
  //consumes tokens from the parser
  void scanLoop(BFParser &parser, addr_offset offset);
  
  //emits optimzied code for scanning left
  //computes: data_ptr = memrchr(mem_start, 0, data_ptr+offset+1-mem_start)
  //  MOV rdi, r13
  //  XOR rsi, rsi
  //  LEA rdx, BYTE PTR [r12+offset+1]
  //  SUB rdx, r13
  //  CALL _memrchr
  //  MOV r12, rax
  void scanLeft(addr_offset offset);
  
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
  void scanRight(addr_offset offset);
  
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
  void multiplyLoop(BFParser &parser, addr_offset offset);

 public:
  //constructor
  BFCompilerX86(asmjit::X86Assembler *assm);
  
  //compiles the brainfuck program into a function that
  //uses the System V x86-64 ABI
  //memory to allocate is passed through rdi
  //returns 0 on success, 1 on failure to allocate memory
  void compile(const char *filename);
};

#endif
