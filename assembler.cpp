#include "compiler.h"
#include <stdio.h>
#include <vector>

using namespace asmjit;

struct Loop {
  Label start;
  Label end;

  Loop(Label _start, Label _end) : start(_start), end(_end) {}
};

void _putchar(uint8_t c) {
  putchar(c);
}

char _getchar() {
  return getchar();
}

//assembles the intermediate code into native assembly
void BFCompiler::assemble(X86Assembler &assembler) {

  std::vector<Loop> loop_stack;
  assembler.mov(x86::r8, (intptr_t) memory);
  
  for (auto t: instructions) {
    switch (t.operation) {
    case BFInst::ADD:
      assembler.add(x86::r8, t.argument);
      break;
      
    case BFInst::SUB:
      assembler.sub(x86::r8, t.argument);
      break;
      
    case BFInst::ADDB:
      assembler.add(x86::byte_ptr(x86::r8, t.offset), t.argument);
      break;
      
    case BFInst::SUBB:
      assembler.sub(x86::byte_ptr(x86::r8, t.offset), t.argument);
      break;
      
    case BFInst::OUT:
      assembler.mov(x86::rax, 1);
      assembler.mov(x86::rdi, 1);
      assembler.lea(x86::rsi, x86::byte_ptr(x86::r8, t.offset));
      assembler.mov(x86::rdx, 1);
      assembler.syscall();
      /*
      assembler.movzx(x86::rdi, x86::byte_ptr(x86::r8));
      assembler.call(imm_ptr(_putchar));
      */
      break;
      
    case BFInst::IN:
      assembler.mov(x86::rax, 0);
      assembler.mov(x86::rdi, 0);
      assembler.lea(x86::rsi, x86::byte_ptr(x86::r8, t.offset));
      assembler.mov(x86::rdx, 1);
      assembler.syscall();
      break;
      
    case BFInst::JMPZ: {
      Loop loop(assembler.newLabel(), assembler.newLabel());

      assembler.cmp(x86::byte_ptr(x86::r8), 0);
      assembler.je(loop.end);
      assembler.bind(loop.start);

      loop_stack.push_back(loop);
    }
      break;
      
    case BFInst::JMPNZ: {
      Loop loop = loop_stack.back();
      loop_stack.pop_back();
	  
      assembler.cmp(x86::byte_ptr(x86::r8), 0);
      assembler.jne(loop.start);
      assembler.bind(loop.end);
    }
      break;
      
    default:
      exit(1);
      //throw UnimplementedOperationException();
      break;
    }
  }

  assembler.ret();
}
