#include "parser.h"
#include <iostream>

using namespace std;

/*
 * Constructor
 * reads source file and stores tokens in buffer
 * set index to 0 (the start)
 */
Parser::Parser(const char* filename) {
  index = 0;
  instructions = "";
  
  ifstream infile = ifstream(filename, ios::in);

  char token;
  while (infile >> token) {
    switch (token) {
    case '>': case '<': case '+': case '-':
    case '.': case ',': case '[': case ']':
      instructions.push_back(token);
    }
  }
  
  infile.close();
}

//returns true if there are more tokens to parse
bool Parser::hasNext() {
  return index != instructions.size();
}

//returns the next token
char Parser::next() {
  return instructions[index++];
}

//returns the next token without consuming it
char Parser::peek() {
  return instructions[index];
}
