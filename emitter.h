#ifndef BRAINFAST_EMITTER_H
#define BRAINFAST_EMITTER_H

#include <list>
#include <vector>
#include <exception>
#include <asmjit/asmjit.h>
#include "intermediate.h"

using namespace asmjit;

class MissingBracketException : public std::exception {
  const char *message;
 public:
  MissingBracketException(const char *msg);
  const char *what();
};

class UnimplementedOperationException : public std::exception {
 public:
  const char *what();
};

//assembles the intermediate code into native assembly
void assemble(X86Assembler &assembler, std::list<Token> tokens, void* memory);

#endif
