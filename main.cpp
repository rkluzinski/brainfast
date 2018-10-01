#include <iostream>
#include <vector>
#include <asmjit/asmjit.h>

#include "parser.h"

using namespace std;
using namespace asmjit;

struct Loop {
  asmjit::Label start;
  asmjit::Label end;

  Loop(asmjit::Label start, asmjit::Label end);
};

Loop::Loop(asmjit::Label _start, asmjit::Label _end) : start(_start), end(_end) {}

void compile(Parser &parser, X86Assembler &assembler, uint8_t *memory) {
  std::vector<Loop> loop_stack;
  assembler.mov(x86::r8, (intptr_t) memory);

  while (parser.hasNext()) {
    switch (parser.next()) {
    case '>':
      assembler.add(x86::r8, 1);
      break;
      
    case '<':
      assembler.sub(x86::r8, 1);
      break;
      
    case '+':
      assembler.add(x86::byte_ptr(x86::r8), 1);
      break;
      
    case '-':
      assembler.sub(x86::byte_ptr(x86::r8), 1);
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
      Loop loop(assembler.newLabel(), assembler.newLabel());

      assembler.cmp(x86::byte_ptr(x86::r8), 0);
      assembler.je(loop.end);
      assembler.bind(loop.start);

      loop_stack.push_back(loop);
    }
      break;
      
    case ']': {
      if (loop_stack.empty()) {
	cout << "Mismatched '['" << endl;
	exit(0);
      }

      Loop loop = loop_stack.back();
      loop_stack.pop_back();
	  
      assembler.cmp(x86::byte_ptr(x86::r8), 0);
      assembler.jne(loop.start);
      assembler.bind(loop.end);
    }
      break;
      
    default: //throws error on non-instruction char
      cout << "Illegal Instruction!" << endl;
      exit(0);
      break;
    }
  }

  if (!loop_stack.empty()) {
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

  uint8_t memory[0x7fff];

  JitRuntime runtime;

  CodeHolder code;
  code.init(runtime.getCodeInfo());

  X86Assembler assembler(&code);

  Parser parser(argv[1]);
  compile(parser, assembler, memory);

  void (*fn)(void);
  Error error = runtime.add(&fn, &code);
  if (error)
    return 1;

  fn();
  return 0;
}
