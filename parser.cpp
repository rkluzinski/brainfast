#include "parser.h"
#include <iostream>
#include <fstream>

//constructor
//reads the file and stores bf instructions in string
BFParser::BFParser(const char *filename) : index(0) {
  std::ifstream infile(filename, std::ios::in);
  char instruction;

  //for checking bracket balance
  int bracket_count = 0;

  if (!infile.is_open()) {
    std::cout << "Could not open file: " << filename << "!" << std::endl;
    exit(0);
  }
  
  while (infile >> instruction) {
    switch (instruction) {
    case '>':
    case '<':
    case '+':
    case '-':
    case '.':
    case ',':
      instructions.push_back(instruction);
      break;
      
    case '[':
      instructions.push_back(instruction);
      bracket_count++;
      break;
      
    case ']':
      instructions.push_back(instruction);
      if (bracket_count-- == 0) {
	std::cout << "Missing opening bracket '['!" << std::endl;
	exit(0);
      }
      
    default: break;
    }
  }

  if (bracket_count != 0) {
    std::cout << "Missing closing bracket ']'!" << std::endl;
    exit(0);
  }
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
//clear loop: [-] or [+]
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
//scan loop: [>] or [<]
bool BFParser::isScanLoop() {
  if (instructions[index] != '[') {
    return false;
  }
  else if (instructions[index+1] != '>' && instructions[index+1] != '<') {
    return false;
  }
  else if (instructions[index+2] != ']') {
    return false;
  }
  else {
    return true;
  }
}

//check if the next sequence is a multiply loop
//multiply loop: [{any sequence of >, <, +, >}] where
//the net pointer offset is zero (number of > == number of <)
//one is subtracted from the first cell each iteration
bool BFParser::isMultiplyLoop() {
  int counter_delta = 0;
  int ptr_delta = 0;

  int i = index;
  bool done = false;
  while (!done) {
    switch(instructions[++i]) {
    case '>': ptr_delta++; break;
    case '<': ptr_delta--; break;
    case '+':
      if (ptr_delta == 0)
	counter_delta++;
      break;
    case '-':
      if(ptr_delta == 0)
	counter_delta--;
      break;
      
    case '.':
    case ',':
    case '[':
      return false;
      
    case ']':
      done = true;
      break;
    }
  }

  return (counter_delta == -1) && (ptr_delta == 0);
}
