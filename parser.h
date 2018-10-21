#ifndef BRAINFAST_PARSER
#define BRAINFAST_PARSER

#include <string>

class BFParser {
 private:
  //index in instructions
  size_t index;
  std::string instructions;

 public:
  //constructor
  //reads the file and stores bf instructions in string
  BFParser(const char *filename);
  ~BFParser();

  //checks if there are more tokens
  bool hasNext();
  //get the next token and advance the parser
  char next();
  //get the next token without advancing the parser
  char peek();
  //consume 'count' tokens without returning them
  void consume(int count);

  //check if the next sequence is a clear loop
  bool isClearLoop();
  //check if the next sequence is a scan loop
  bool isScanLoop();
  //check if the next sequence is a multiply loop
  bool isMultiplyLoop();
};

#endif
