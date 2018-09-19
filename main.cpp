#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

enum Opcode {
  ADD_PTR,
  SUB_PTR,
  ADD,
  SUB,
  OUT,
  IN,
  JMP_ZERO,
  JMP_NZERO
};

struct BFOp {
  Opcode opcode;
  size_t argument;

  BFOp(Opcode opcode_param, size_t arg_param) {
    opcode = opcode_param;
    argument = arg_param;
  }
};

//structure to hold the parsed brainfuck program
struct BFProgram {
  vector<BFOp> instructions;
  size_t length;
};

//parses the brainfuck progra from a file
BFProgram parse_file(string filename) {
  ifstream infile;
  infile.open(filename, ios::in);

  BFProgram program;
  char byte;
  
  while (infile >> byte) {
    switch (byte) {
    case '>':
      program.instructions.push_back(BFOp(ADD_PTR, 1));
      break;
      
    case '<':
      program.instructions.push_back(BFOp(SUB_PTR, 1));
      break;
      
    case '+':
      program.instructions.push_back(BFOp(ADD, 1));
      break;
      
    case '-':
      program.instructions.push_back(BFOp(SUB, 1));
      break;
      
    case '.':
      program.instructions.push_back(BFOp(OUT, 0));
      break;
      
    case ',':
      program.instructions.push_back(BFOp(IN, 0));
      break;
      
    case '[':
      program.instructions.push_back(BFOp(JMP_ZERO, 0));
      break;
      
    case ']':
      program.instructions.push_back(BFOp(JMP_NZERO, 0));
      break;
      
    default: //remove all other characters
      break;
    }
  }
  
  infile.close();
  
  program.length = program.instructions.size();
  
  return program;
}

//computes the jumptable for a brainfuck program
void compute_jumps(BFProgram &program) {
  vector<size_t> stack;

  for (size_t pc = 0; pc < program.length; pc++) {
    switch (program.instructions[pc].opcode) {
    case JMP_ZERO:
      stack.push_back(pc);
      break;
      
    case JMP_NZERO:
      if (stack.size() == 0) {
	cout << "Missing '['" << endl;
	exit(0);
      }
    
      program.instructions[pc].argument = stack.back();
      program.instructions[stack.back()].argument = pc;
      stack.pop_back();
      break;
      
    default:
      break;
    }
  }

  if (stack.size() != 0) {
    cout << "Missing ']'" << endl;
    exit(0);
  }
			     
}

//interprets a brainfuck program
void interpret_bf(const BFProgram &program) {
  size_t pc = 0;

  unsigned char memory[0x7fff] = {0};
  unsigned char *pointer = memory;

  while (pc < program.instructions.size()) {
    switch (program.instructions[pc].opcode) {
    case ADD_PTR: //increment data pointer
      pointer += program.instructions[pc].argument;
      break;
      
    case SUB_PTR: //decrement data pointer
      pointer -= program.instructions[pc].argument;
      break;
      
    case ADD: //increment byte
      *pointer += program.instructions[pc].argument;
      break;
      
    case SUB: //decrement byte
      *pointer -= program.instructions[pc].argument;
      break;
      
    case OUT: //output byte
      cout << (char) *pointer;
      break;
      
    case IN: //store one byte
      cin >> *pointer;
      break;
      
    case JMP_ZERO: //if byte is zero, jump to matching ']'
      if (*pointer == 0)
	pc = program.instructions[pc].argument;
      break;
      
    case JMP_NZERO: //if byte is nonzero, jump back to matching '['
      if (*pointer)
	pc = program.instructions[pc].argument;
      break;
      
    default: //all other characters ignored
      break;
    }

    pc++;
  }
}

int main(int argc, char **argv) {
  if (argc != 2) {
    cout << "usage: ./brainfast filename\n" << endl;
    return 0;
  }

  BFProgram program = parse_file(argv[1]);
  compute_jumps(program);
  interpret_bf(program);
  
  return 0;
}
