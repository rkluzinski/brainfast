#include <iostream>
#include <asmjit/asmjit.h>
#include "compiler.h"

using namespace std;
using namespace asmjit;

#define DEFAULT_MEMORY_SIZE 65536

int main(int argc, char **argv) {
  //default values
  int memory_size = DEFAULT_MEMORY_SIZE;
  char *filename = NULL;

  //basic way to parse command line arguments
  if (argc < 2) {
    cout << "usage: " << argv[0] << " [MEMORY_SIZE] filename" << endl;
    exit(0);
  }
  else if (argc == 2) {
    filename = argv[1];
  }
  else {
    memory_size = atoi(argv[1]);
    filename = argv[2];
  }

  //validate arguments passed
  if (memory_size <= 0) {
    cout << "Invalid memory size!" << endl;
    exit(0);
  }

  //prepares the asmjit environment
  JitRuntime runtime;
  CodeHolder code;
  code.init(runtime.getCodeInfo());
  X86Assembler assembler(&code);

  //compiles the program
  BFCompilerX86 compiler(&assembler);
  compiler.compile(filename);

  //creates a function pointer to assembled code
  BFProgram fn;
  Error error = runtime.add(&fn, &code);
  if (error) {
    cout << "asmjit runtime error!" << endl;
    return 0;
  }

  //calls brainfuck program
  if (fn(memory_size) == 1) {
    cout << "failed to allocate memory!" << endl;
  }
  
  return 0;
}
