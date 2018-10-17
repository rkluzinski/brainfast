#include <iostream>
#include <list>
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

  BFCompiler compiler;
  compiler.compile(argv[1]);

  //compiler.merge_addb_subb();
  //compiler.postpone_movements();
  
  compiler.assemble(assembler);

  void (*fn)(void);
  Error error = runtime.add(&fn, &code);
  if (error)
    return 1;

  fn();
  return 0;
}
