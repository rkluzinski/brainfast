#include "compiler.h"
#include <vector>
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

//reads the source file and returns a string of all valid brainfuck instructions
std::string parse_file(const char *filename) {
  std::ifstream infile(filename, std::ios::in);

  std::string source;
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
      source.push_back(instruction);
      
    default: break;
    }
  }

  return source;
}

//combines sequential '+' and '-' operaters into a single add instruction
BFInst createArithmeticInst(std::string source, size_t index, int offset) {
  uint8_t sum = 0;

  for (;; index++) {
    if (source[index] == '+')
      sum += 1;
    else if (source[index] == '-')
      sum -= 1;
    else
      break;
  }

  return BFInst(BFInst::ADDB, sum, offset);
}

//creates a pointer movement instruction with the given offset
BFInst createMovementInst(int offset) {
  if (offset > 0)
    return BFInst(BFInst::ADD, offset);
  else
    return BFInst(BFInst::SUB, -offset);
}

//parses the file and returns a string of all the instructions
void BFCompiler::compile(const char* filename) {
  std::string source = parse_file(filename);

  int offset = 0;
  int loop_offset = 0;
  std::vector<int> stack;

  for (size_t i = 0; i < source.size(); i++) {
    switch (source[i]) {
    case '>':
      offset += 1;
      loop_offset += 1;
      break;
      
    case '<':
      offset -= 1;
      loop_offset -= 1;
      break;
      
    case '+':
    case '-':
      instructions.push_back(createArithmeticInst(source, i, offset));
      while (source[i+1] == '+' || source[i+1] == '-')
	i++;
      break;
      
    case '.': instructions.push_back(BFInst(BFInst::OUT, 0, offset)); break;
    case ',': instructions.push_back(BFInst(BFInst::IN, 0, offset)); break;
      
    case '[':
      stack.push_back(loop_offset);
      loop_offset = 0;
      
      instructions.push_back(BFInst(BFInst::JMPZ, 0, offset));
      break;
      
    case ']':
      if (loop_offset != 0)
	instructions.push_back(createMovementInst(loop_offset));

      offset -= loop_offset;

      if (stack.empty())
	//throw exception
	exit(1);

      loop_offset = stack.back();
      stack.pop_back();
      
      instructions.push_back(BFInst(BFInst::JMPNZ, 0, offset));
      break;
      
    default: break;
    }
  }

  if (!stack.empty())
    //throw exception
    exit(1);
}
