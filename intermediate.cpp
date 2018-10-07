#include "intermediate.h"

//intermediate representation token constructors
Token::Token(Operation op) : operation(op), argument(0), offset(0) {}
Token::Token(Operation op, int arg) : operation(op), argument(arg), offset(0) {}
Token::Token(Operation op, int arg, int off) : operation(op), argument(arg), offset(off) {}

//compile brainfuck instructions to intermediate operations
std::list<Token> compile(std::string instructions) {
  std::list<Token> tokens = std::list<Token>();

  for (auto instruction: instructions) {
    switch (instruction) {
    case '>': tokens.push_back(Token(Token::ADD_PTR, 1)); break;
    case '<': tokens.push_back(Token(Token::SUB_PTR, 1)); break;
    case '+': tokens.push_back(Token(Token::ADD, 1)); break;
    case '-': tokens.push_back(Token(Token::SUB, 1)); break;
    case '.': tokens.push_back(Token(Token::OUT)); break;
    case ',': tokens.push_back(Token(Token::IN)); break;
    case '[': tokens.push_back(Token(Token::JMPZ)); break;
    case ']': tokens.push_back(Token(Token::JMPNZ)); break;
    default:  tokens.push_back(Token(Token::NOOP)); break;
    }
  }

  return tokens;
}
