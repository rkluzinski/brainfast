#include "compiler.h"

//combines sequential adds and subtracts
void BFCompiler::merge_addb_subb() {
  BFInst inst(BFInst::ADDB);
  auto i = instructions.begin();
  
  while (i != instructions.end()) {
    if (i->operation == BFInst::ADDB) {
      inst.argument += i->argument;
      instructions.erase(i++);
    }
    else if (i->operation == BFInst::SUBB) {
      inst.argument -= i->argument;
      instructions.erase(i++);
    }
    else if (inst.argument != 0) {
      instructions.insert(i++, inst);
      inst.argument = 0;
    }
    else {
      i++;
    }
  }
}

void BFCompiler::clear_loops() {
  auto i = instructions.begin();
  while (i != instructions.end()) {
    i++;
  }
}

BFInst pointer_op(int displacement) {
  if (displacement > 0)
    return BFInst(BFInst::ADD, displacement);
  else
    return BFInst(BFInst::SUB, -displacement);
}

int BFCompiler::recursive_postpone(std::list<BFInst>::iterator &i, int displacement) {
  int loop_pointer = 0;
  
  while (i != instructions.end()) {
    if (i->operation == BFInst::ADD) {
      displacement += i->argument;
      loop_pointer += i->argument;
      instructions.erase(i++);
    }
    else if (i->operation == BFInst::SUB) {
      displacement -= i->argument;
      loop_pointer -= i->argument;
      instructions.erase(i++);
    }
    else if (i->operation == BFInst::JMPZ) {
      (i++)->offset = displacement;
      displacement = recursive_postpone(i, displacement);
    }
    else if (i->operation == BFInst::JMPNZ) {
      displacement -= loop_pointer;
      i->offset = displacement;
      if (loop_pointer != 0)
	instructions.insert(i, pointer_op(loop_pointer));
      i++;
      return displacement;
    }
    else {
      (i++)->offset = displacement;
    }
  }

  return 0;
}

//postpones pointer movements until they are necesary
void BFCompiler::postpone_movements() {
  auto i = instructions.begin();
  recursive_postpone(i, 0);
}

