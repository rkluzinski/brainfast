#include "compiler.h"

//combines sequential adds and subtracts
void BFCompiler::combine_arithmetic() {
  uint8_t sum = 0;
  auto i = instructions.begin();
  
  while (i != instructions.end()) {
    switch (i->operation) {
    case BFInst::ADDB:
      sum += i->argument;
      instructions.erase(i++);
      break;
      
    case BFInst::SUBB:
      sum -= i->argument;
      instructions.erase(i++);
      break;

    default:
      if (sum != 0) {
	BFInst inst = BFInst(BFInst::ADDB, sum);
	instructions.insert(i, inst);
	sum = 0;
      }
      i++;
      break;
    }
  }
}

void BFCompiler::clear_loops() {
  auto i = instructions.begin();
  while (i != instructions.end()) {
    i++;
  }
}

//postpones pointer movements until they are necesary
void BFCompiler::postpone_movements() {
  int virtual_ptr = 0;
  auto i = instructions.begin();
 
  while (i != instructions.end()) {
    switch (i->operation) {
    case BFInst::ADD:
      virtual_ptr += i->argument;
      instructions.erase(i++);
      break;
      
    case BFInst::SUB:
      virtual_ptr -= i->argument;
      instructions.erase(i++);
      break;

    case BFInst::ADDB:
    case BFInst::SUBB:
    case BFInst::OUT:
    case BFInst::IN:
    case BFInst::MOV:
      i->offset = virtual_ptr;
      i++;
      break;

    default:
      if (virtual_ptr != 0) {
	BFInst inst = BFInst(BFInst::ADD, virtual_ptr);

	if (virtual_ptr < 0) {
	  inst.operation = BFInst::SUB;
	  inst.argument = -virtual_ptr;
	}
	
	instructions.insert(i, inst);
	virtual_ptr = 0;
      }
      i++;
      break;
    }
  }
}

