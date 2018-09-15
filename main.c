#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

//prints error message if malloc fails to allocate
void check_malloc(void *ptr) {
  if (ptr == NULL) {
    fprintf(stderr, "Failed to allocate memory.\n");
    exit(0);
  }
}

int main(int argc, char **argv) {
  if (argc != 2) {
    //ELF_NAME defined in Makefile
    fprintf(stdout, "usage: %s filename\n", ELF_NAME);
    exit(0);
  }

  //open file
  FILE *fp = NULL;
  fp = fopen(argv[1], "rb");
  if (fp == NULL) {
    fprintf(stderr, "Couldn't open file: %s\n", argv[1]);
    exit(0);
  }

  //gets the length of the file
  fseek(fp, 0, SEEK_END);
  long fsize = ftell(fp);
  rewind(fp);

  //allocate memory for source buffer
  char *buffer = (char*) malloc(fsize * sizeof(char));
  check_malloc(buffer);

  //reads file into buffer
  if (fread(buffer, fsize, 1, fp) != 1) {
    fprintf(stderr, "Failed to read file.\n");
    exit(0);
  }

  int *stack = (int*) malloc(fsize * sizeof(int)), stackp = 0;
  int *jump_table = (int*) malloc(fsize * sizeof(int));

  check_malloc(stack);
  check_malloc(jump_table);

  //build the jump table
  for (int i = 0; i < fsize; i++) {
    if (buffer[i] == '[') {
      stack[stackp++] = i;
    }
    else if (buffer[i] == ']') {
      int index = stack[--stackp];
      jump_table[index] = i;
      jump_table[i] = index;
    }
  }

  free(stack);
  if (stackp != 0) {
    fprintf(stderr, "Mismatched brackets in code.\n");
    exit(0);
  }

  //32767 bytes of memory
  char memory[0x7fff] = {0};
  char *pointer = memory;

  //interpret
  for (int i = 0; i < fsize; i++) {
    switch (buffer[i]) {
    case '>': //increment data pointer
      ++pointer;
      break;
      
    case '<': //decrement data pointer
      --pointer;
      break;
      
    case '+': //increment byte
      ++*pointer;
      break;
      
    case '-': //decrement byte
      --*pointer;
      break;
      
    case '.': //output byte
      putchar(*pointer);
      break;
      
    case ',': //store one byte
      *pointer = getchar();
      break;
      
    case '[': //if byte is zero, jump to matching ']'
      i = (*pointer == 0) ? jump_table[i] : i;
      break;
      
    case ']': //if byte is nonzero, jump back to matching '['
      i = (*pointer) ? jump_table[i] : i;
      break;
      
    default: //all other characters ignored
      break;
    }
  }

  free(jump_table);
  free(buffer);

  return 0;
}
