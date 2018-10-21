#include <iostream>
#include <asmjit/asmjit.h>

#include "compiler.h"

using namespace std;
using namespace asmjit;

int main(int argc, char **argv) {
  if (argc != 2) {
    cout << "usage: ./brainfast filename" << endl;
    return 0;
  }

  JitRuntime runtime;

  CodeHolder code;
  code.init(runtime.getCodeInfo());

  X86Assembler assembler(&code);

  BFCompilerX86 compiler(&assembler);
  compiler.compile(argv[1]);

  void (*fn)(void);
  Error error = runtime.add(&fn, &code);
  if (error)
    return 1;

  fn();
  return 0;
}
