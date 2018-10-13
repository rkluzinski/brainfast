#include "compiler.h"

/*
//combines sequential adds and subtracts
void combine_arithmetic(std::list<Token> &tokens) {

  int sum = 0;
  
  auto i = tokens.begin();
  while (i != tokens.end()) {
    switch (i->operation) {
    case Token::ADD_B:
      sum += i->argument;
      tokens.erase(i++);
      break;
      
    case Token::SUB_B:
      sum -= i->argument;
      tokens.erase(i++);
      break;

    default:
      if (sum != 0) {
	Token t = Token(Token::ADD_B, sum);
	tokens.insert(i, t);
	sum = 0;
      }
      i++;
      break;
    }
  }
}

//postpones pointer movements until they are necesary
void postpone_movements(std::list<Token> &tokens) {
  int virtual_ptr = 0;

  auto i = tokens.begin();
  while (i != tokens.end()) {
    switch (i->operation) {
    case Token::ADD:
      virtual_ptr += i->argument;
      tokens.erase(i++);
      break;
      
    case Token::SUB:
      virtual_ptr -= i->argument;
      tokens.erase(i++);
      break;

    case Token::ADD_B:
    case Token::SUB_B:
      i->offset = virtual_ptr;
      i++;
      break;

    default:
      if (virtual_ptr != 0) {
	Token t = Token(Token::ADD, virtual_ptr);
	tokens.insert(i, t);
	virtual_ptr = 0;
      }
      i++;
      break;
    }
  }
}
*/
