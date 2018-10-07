#include "emitter.h"

MissingBracketException::MissingBracketException(const char *msg) : message(msg) {}

const char *MissingBracketException::what() {
  return message;
}

const char *UnimplementedOperationException::what() {
  return "An unimplemented instruction has been encountered\n";
}

struct Loop {
  Label start;
  Label end;

  Loop(Label _start, Label _end) : start(_start), end(_end) {}
};

//assembles the intermediate code into native assembly
void assemble(X86Assembler &assembler, std::list<Token> tokens, void *memory) {

  std::vector<Loop> loop_stack;
  assembler.mov(x86::r8, (intptr_t) memory);
  
  for (auto t: tokens) {
    switch (t.operation) {
    case Token::ADD_PTR:
      assembler.add(x86::r8, t.argument);
      break;
      
    case Token::SUB_PTR:
      assembler.sub(x86::r8, t.argument);
      break;
      
    case Token::ADD:
      assembler.add(x86::byte_ptr(x86::r8, t.offset), t.argument);
      break;
      
    case Token::SUB:
      assembler.sub(x86::byte_ptr(x86::r8, t.offset), t.argument);
      break;
      
    case Token::OUT:
      assembler.mov(x86::rax, 1);
      assembler.mov(x86::rdi, 1);
      assembler.mov(x86::rsi, x86::r8);
      assembler.mov(x86::rdx, 1);
      assembler.syscall();
      break;
      
    case Token::IN:
      assembler.mov(x86::rax, 0);
      assembler.mov(x86::rdi, 0);
      assembler.mov(x86::rsi, x86::r8);
      assembler.mov(x86::rdx, 1);
      assembler.syscall();
      break;
      
    case Token::JMPZ: {
      Loop loop(assembler.newLabel(), assembler.newLabel());

      assembler.cmp(x86::byte_ptr(x86::r8), 0);
      assembler.je(loop.end);
      assembler.bind(loop.start);

      loop_stack.push_back(loop);
    }
      break;
      
    case Token::JMPNZ: {
      if (loop_stack.empty())
        throw MissingBracketException("Missing '['");

      Loop loop = loop_stack.back();
      loop_stack.pop_back();
	  
      assembler.cmp(x86::byte_ptr(x86::r8), 0);
      assembler.jne(loop.start);
      assembler.bind(loop.end);
    }
      break;
      
    default:
      throw UnimplementedOperationException();
      break;
    }
  }

  if (!loop_stack.empty())
    throw MissingBracketException("Missing ']'");

  assembler.ret();
}
