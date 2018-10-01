#ifndef PARSER_H
#define PARSER_H

#include <fstream>
#include <string>

class Parser {
 public://private:
  size_t index;
  std::string instructions;
  
 public:
  Parser(const char* filename);

  bool hasNext();
  char next();
  char peek();
};

#endif
