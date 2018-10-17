#ifndef BRAINFAST_COMPILER_H
#define BRAINFAST_COMPILER_H

#include <string>
#include <list>
#include <asmjit/asmjit.h>

//intermediate representation instructions
struct BFInst {
  enum Operation {
    NOP,
    ADD,
    SUB,
    ADDB,
    SUBB,
    OUT,
    IN,
    JMPZ,
    JMPNZ,
    MOV,
    FMA,
  } operation;
  uint8_t argument;
  uint64_t offset;

  //implemented in compiler.cpp
  BFInst(Operation op);
  BFInst(Operation op, int arg);
  BFInst(Operation op, int arg, int offset);
};

class BFCompiler {
 private:
  std::list<BFInst> instructions;
  unsigned char *memory;
  
 public:
  BFCompiler();
  ~BFCompiler();

  //implemented in compiler.cpp
  //converts source file to intermediate instructions
  void compile(const char* filename);

  //implemented in optimze.cpp
  void merge_addb_subb();
  void clear_loops();

  int recursive_postpone(std::list<BFInst>::iterator &i, int displacement);
  void postpone_movements();

  //implemented in assembler.cpp
  void assemble(asmjit::X86Assembler &assembler);
};

#endif
