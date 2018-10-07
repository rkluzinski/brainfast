#include "parser.h"

//parses the file and returns a string of all the instructions
std::string parse_file(std::string filename) {
  std::ifstream infile(filename, std::ios::in);

  std::string instructions;
  char instruction;

  while(infile >> instruction) {
    switch (instruction) {
    case '>': case '<': case '+': case '-':
    case '.': case ',': case '[': case ']':
      instructions.push_back(instruction);
      break;
      
    default:
      break;
    }
  }

  return instructions;
}
