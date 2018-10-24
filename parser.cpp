#include "parser.h"
#include <fstream>

//constructor
//reads the file and stores bf instructions in string
BFParser::BFParser(const char *filename) {
  std::ifstream infile(filename, std::ios::in);
  char instruction;
  
  while (infile >> instruction) {
    switch (instruction) {
    case '>':
    case '<':
    case '+':
    case '-':
    case '.':
    case ',':
    case '[':
    case ']':
      instructions.push_back(instruction);
      
    default: break;
    }
  }

  index = 0;
}

//destructor
BFParser::~BFParser() {
  return;
}

//checks if there are more tokens
bool BFParser::hasNext() {
  return index != instructions.size();
}

//get the next token and advance the parser
char BFParser::next() {
  return instructions[index++];
}

//get the next token without advancing the parser
char BFParser::peek() {
  return instructions[index];
}

//consume 'count' tokens without returning them
void BFParser::consume(int count) {
  index += count;
}

//check if the next sequence is a clear loop
bool BFParser::isClearLoop() {
  if (instructions[index] != '[') {
    return false;
  }
  else if (instructions[index+1] != '+' && instructions[index+1] != '-') {
    return false;
  }
  else if (instructions[index+2] != ']') {
    return false;
  }
  else {
    return true;
  }
}

//check if the next sequence is a scan loop
bool BFParser::isScanLoop() {
  return false;
}

//check if the next sequence is a multiply loop
bool BFParser::isMultiplyLoop() {
  return false;
}
