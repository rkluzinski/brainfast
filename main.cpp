#include <iostream>
#include <asmjit/asmjit.h>
#include "compiler.h"

using namespace std;
using namespace asmjit;

int main(int argc, char **argv) {
  if (argc < 2) {
    cout << "usage: ./brainfast filename" << endl;
    return 0;
  }

  JitRuntime runtime;

  //FileLogger logger(stdout);
  
  CodeHolder code;
  code.init(runtime.getCodeInfo());
  //code.setLogger(&logger);
  
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
    cout << "malloc failed!" << endl;
  }
  
  return 0;
}
