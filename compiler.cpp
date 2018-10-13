#include "compiler.h"
#include <fstream>
#include <stdlib.h>

//intermediate representation token constructors
BFInst::BFInst(Operation op) : operation(op), argument(0), offset(0) {}
BFInst::BFInst(Operation op, int arg) : operation(op), argument(arg), offset(0) {}
BFInst::BFInst(Operation op, int arg, int off) : operation(op), argument(arg), offset(off) {}

BFCompiler::BFCompiler() {
  memory = (unsigned char*) malloc(0x7fff * sizeof(unsigned char*));
}

BFCompiler::~BFCompiler() {
  free(memory);
}

//parses the file and returns a string of all the instructions
void BFCompiler::parse(const char* filename) {
  std::ifstream infile(filename, std::ios::in);
  int bracket_counter = 0;
  char instruction;

  while(infile >> instruction) {
    switch (instruction) {
    case '>': instructions.push_back(BFInst(BFInst::ADD, 1));  break;
    case '<': instructions.push_back(BFInst(BFInst::SUB, 1));  break;
    case '+': instructions.push_back(BFInst(BFInst::ADDB, 1)); break;
    case '-': instructions.push_back(BFInst(BFInst::SUBB, 1)); break;
    case '.': instructions.push_back(BFInst(BFInst::OUT));     break;
    case ',': instructions.push_back(BFInst(BFInst::IN));      break;
      
    case '[':
      instructions.push_back(BFInst(BFInst::JMPZ));
      bracket_counter++;
      break;
      
    case ']':
      instructions.push_back(BFInst(BFInst::JMPNZ));
      bracket_counter--;
      if (bracket_counter < 0)
	//throw exception
	exit(1);
      break;
      
    default: break;
    }
  }

  if (bracket_counter)
    //throw exception
    exit(1);
}
