#include <iostream>
#include <asmjit/asmjit.h>
#include "compiler.h"

using namespace std;
using namespace asmjit;

#define DEFAULT_MEMORY_SIZE 0xffff

//parses the command line arguments
int parse_arguments() {

}

int main(int argc, char **argv) {
  if (argc < 2) {
    cout << "usage: " << argv[0] << " [-m MEMORY_SIZE] filename" << endl;
    return 0;
  }

  int memory_size = DEFAULT_MEMORY_SIZE;
  const char *filename = argv[1];

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
