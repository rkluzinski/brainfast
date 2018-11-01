#include <iostream>
#include <asmjit/asmjit.h>
#include "compiler.h"

using namespace std;
using namespace asmjit;

int main(int argc, char **argv) {
  if (argc < 2) {
    cout << "usage: " << argv[0] << " [-m MEMORY_SIZE] filename" << endl;
    return 0;
  }

  JitRuntime runtime;
  CodeHolder code;
  code.init(runtime.getCodeInfo());
  X86Assembler assembler(&code);

  BFCompilerX86 compiler(&assembler);
  compiler.compile(argv[1]);

  BFProgram fn;
  Error error = runtime.add(&fn, &code);
  if (error) {
    cout << "asmjit runtime error!" << endl;
    return 0;
  }

  if (fn(0xffff) == 1) {
    cout << "failed to allocate memory!" << endl;
  }
  
  return 0;
}
