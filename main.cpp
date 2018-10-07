#include <iostream>
#include <list>
#include <asmjit/asmjit.h>

#include "parser.h"
#include "intermediate.h"
#include "optimize.h"
#include "emitter.h"

using namespace std;
using namespace asmjit;

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

  //parse the source file
  string instructions = parse_file(argv[1]);
  std::list<Token> tokens = compile(instructions);

  //apply optimizations
  combine_arithmetic(tokens);
  postpone_movements(tokens);

  //assemble the file
  try {
    assemble(assembler, tokens, memory);
  }
  catch (MissingBracketException mbe) {
    cout << "Assembly failed!" << endl;
    cout << mbe.what() << endl;
    return 0;
  }
  catch (UnimplementedOperationException uoe) {
    cout << "Assembly failed!" << endl;
    cout << uoe.what() << endl;
    return 0;
  }

  void (*fn)(void);
  Error error = runtime.add(&fn, &code);
  if (error)
    return 1;

  fn();
  return 0;
}
