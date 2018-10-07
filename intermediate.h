#ifndef BRAINFAST_INTERMEDIATE_H
#define BRAINFAST_INTERMEDIATE_H

#include <string>
#include <list>

//intermediate representation token class
struct Token {
  enum Operation {
    NOOP,
    ADD_PTR,
    SUB_PTR,
    ADD,
    SUB,
    OUT,
    IN,
    JMPZ,
    JMPNZ,
  } operation;
  int argument;
  int offset;

  Token(Operation op);
  Token(Operation op, int arg);
  Token(Operation op, int arg, int offset);
};

//compile brainfuck instructions to intermediate operations
std::list<Token> compile(std::string instructions);

#endif
