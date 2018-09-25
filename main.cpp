#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <asmjit/asmjit.h>

using namespace std;
using namespace asmjit;

//32768 8-bit cells
uint8_t memory[0x7FFF] = {0};

//parses brainfuck code and removes all the non-code chars
string parse_file(string filename) {
  ifstream infile;
  infile.open(filename, ios::in);

  string instructions;

  char temp;
  while (infile >> temp) {
    switch (temp) {
    case '>': case '<': case '+': case '-':
    case '.': case ',': case '[': case ']':
      instructions.push_back(temp);
      
    default: //remove all other characters
      break;
    }
  }
  
  infile.close();
  return instructions;
}

int count_sequential(string s, char c, int i) {
  int count = 0;
  while (s[i++] == c) {
    count++;
  }
  return count;
}

//compiles the brainfuck program to x64 assembly code
void compile_to_x64(string instructions, X86Assembler &assembler) {

  vector<Label> stack;
  assembler.mov(x86::r8, (intptr_t) memory);

  for (size_t i = 0; i < instructions.size(); i++) {
    auto instruction = instructions[i];
    int count = count_sequential(instructions, instruction, i);
    
    switch (instruction) {
    case '>':
      assembler.add(x86::r8, count);
      i += count - 1;
      break;
      
    case '<':
      assembler.sub(x86::r8, count);
      i += count - 1;
      break;
      
    case '+':
      assembler.add(x86::byte_ptr(x86::r8), count & 0xff);
      i += count - 1;
      break;
      
    case '-':
      assembler.sub(x86::byte_ptr(x86::r8), count & 0xff);
      i += count - 1;
      break;
      
    case '.':
      assembler.mov(x86::rax, 1);
      assembler.mov(x86::rdi, 1);
      assembler.mov(x86::rsi, x86::r8);
      assembler.mov(x86::rdx, 1);
      assembler.syscall();
      break;
      
    case ',':
      assembler.mov(x86::rax, 0);
      assembler.mov(x86::rdi, 0);
      assembler.mov(x86::rsi, x86::r8);
      assembler.mov(x86::rdx, 1);
      assembler.syscall();
      break;
      
    case '[': {
      Label open = assembler.newLabel();
      Label close = assembler.newLabel();

      assembler.cmp(x86::byte_ptr(x86::r8), 0);
      assembler.je(close);
      assembler.bind(open);

      stack.push_back(close);
      stack.push_back(open);
    }
      break;
      
    case ']':
      if (stack.empty()) {
	cout << "Mismatched '['" << endl;
	exit(0);
      }
	  
      assembler.cmp(x86::byte_ptr(x86::r8), 0);
      assembler.jne(stack.back());
      stack.pop_back();

      assembler.bind(stack.back());
      stack.pop_back();
      break;
      
    default: //throws error on non-instruction char
      cout << "Illegal Instruction: " << instruction << endl;
      exit(0);
      break;
    }
  }

  if (!stack.empty()) {
    cout << "Mismatched ']'" << endl;
    exit(0);
  }

  assembler.ret();
}

int main(int argc, char **argv) {
  if (argc != 2) {
    cout << "usage: ./brainfast filename" << endl;
    return 0;
  }

  JitRuntime runtime;

  CodeHolder code;
  code.init(runtime.getCodeInfo());

  X86Assembler assembler(&code);

  string instructions = parse_file(argv[1]);
  compile_to_x64(instructions, assembler);

  void (*fn)(void);
  Error error = runtime.add(&fn, &code);
  if (error)
    return 1;

  fn();
  return 0;
}
