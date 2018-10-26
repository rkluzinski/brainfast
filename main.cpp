#include <iostream>
#include "compiler.h"

using namespace std;

int main(int argc, char **argv) {
  if (argc != 2) {
    cout << "usage: ./brainfast filename" << endl;
    return 0;
  }

  BFCompilerX86 compiler;
  BFProgram fn = compiler.compile(argv[1]);

  if (fn == NULL) {
    cout << "asmjit runtime error!" << endl;
    return 0;
  }

  if (fn(0xffff) == 1) {
    cout << "malloc failed!" << endl;
  }
  
  return 0;
}
