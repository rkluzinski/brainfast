#ifndef BRAINFAST_OPTIMIZER_H
#define BRAINFAST_OPTIMIZER_H

#include <list>
#include "intermediate.h"

//combines sequential adds and subtracts
void combine_arithmetic(std::list<Token> &tokens);

//postpones pointer movements until they are necesary
void postpone_movements(std::list<Token> &tokens);

#endif
