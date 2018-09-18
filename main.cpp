#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

//structure to hold the parsed brainfuck program
struct BFProgram {
  string instructions;
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
    case '<':
    case '+':
    case '-':
    case '.':
    case ',':
    case '[':
    case ']':
      //add valid instructions to program
      program.instructions.push_back(byte);
      
    default: //remove all other characters
      break;
    }
  }
  
  infile.close();
  
  return program;
}

//computes the jumptable for a brainfuck program
vector<size_t> compute_jumptable(const BFProgram &program) {
  size_t pc = 0;
  size_t length = program.instructions.size();

  vector<size_t> stack;
  vector<size_t> jumptable = vector<size_t>(length);

  for (auto op: program.instructions) {
    switch (program.instructions[pc]) {
    case '[':
      stack.push_back(pc);
      break;
      
    case ']':
      jumptable[stack.back()] = pc;
      jumptable[pc] = stack.back();
      stack.pop_back();
      break;
      
    default:
      break;
    }

    pc++;
  }

  return jumptable;
}

//interprets a brainfuck program
void interpret_bf(const BFProgram &program) {
  size_t pc = 0;
  size_t length = program.instructions.size();

  vector<size_t> jumptable = compute_jumptable(program);

  unsigned char memory[0x7fff] = {0};
  unsigned char *pointer = memory;

  while (pc < length) {
    switch (program.instructions[pc]) {
    case '>': //increment data pointer
      ++pointer;
      break;
      
    case '<': //decrement data pointer
      --pointer;
      break;
      
    case '+': //increment byte
      ++*pointer;
      break;
      
    case '-': //decrement byte
      --*pointer;
      break;
      
    case '.': //output byte
      cout << (char) *pointer;
      break;
      
    case ',': //store one byte
      cin >> *pointer;
      break;
      
    case '[': //if byte is zero, jump to matching ']'
      if (*pointer == 0)
	pc = jumptable[pc];
      break;
      
    case ']': //if byte is nonzero, jump back to matching '['
      if (*pointer)
	pc = jumptable[pc];
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
  interpret_bf(program);
  return 0;
}
