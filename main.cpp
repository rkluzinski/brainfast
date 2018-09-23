#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <string.h>
#include <sys/mman.h>

using namespace std;

//parses brainfuck code and removes all the non-code chars
//returns a string of all instructions
string parse_file(string filename) {
  ifstream infile;
  infile.open(filename, ios::in);

  string instructions = "";

  char temp;
  while (infile >> temp) {
    switch (temp) {
    case '>':
    case '<':
    case '+':
    case '-':
    case '.':
    case ',':
    case '[':
    case ']':
      instructions.push_back(temp);
      
    default: //remove all other characters
      break;
    }
  }
  
  infile.close();
  return instructions;
}

void push_bytes(vector<uint8_t> &v, initializer_list<uint8_t> bytes) {
  for (auto byte: bytes) {
    v.push_back(byte);
  }
}

void fill_offset(vector<uint8_t> &v, int index, uint32_t offset) {
  for (int i = 0; i < 4; i++) {
    v[index + i] = (uint8_t) offset;
    offset >>= 8;
  }
}

//compiles the brainfuck program to x64 assembly code
//returns a vector containing the x64 machine code
vector<uint8_t> compile_to_x64(string instructions, uint8_t *memory) {
  vector<uint8_t> x64_code;
  vector<uint32_t> jump_stack;

  push_bytes(x64_code, {0x49,0xB8});
  uint64_t memory_address = (intptr_t) memory;
  for (int i = 0; i < 8; i++) {
    x64_code.push_back((uint8_t) memory_address);
    memory_address >>= 8;
  }

  for (auto instruction: instructions) {
    switch (instruction) {
    case '>':
      //add r8,1
      push_bytes(x64_code, {0x49,0x83,0xC0,0x01});
      break;
      
    case '<':
      //sub r8,1
      push_bytes(x64_code, {0x49,0x83,0xE8,0x01});
      break;
      
    case '+':
      //add BYTE PTR [r8], 1
      push_bytes(x64_code, {0x41,0x80,0x00,0x01});
      break;
      
    case '-':
      //sub BYTE PTR [r8], 1
      push_bytes(x64_code, {0x41,0x80,0x28,0x01});
      break;
      
    case '.':
      //movabs rax,1 ;write
      //movabs rdi,1 ;stdout
      //mov rsi,r8   ;buffer
      //movabs rdx,1 ;count
      //syscall
      push_bytes(x64_code, {0x48,0xB8,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00});
      push_bytes(x64_code, {0x48,0xBF,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00});
      push_bytes(x64_code, {0x4C,0x89,0xC6});
      push_bytes(x64_code, {0x48,0xBA,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00});
      push_bytes(x64_code, {0x0F,0x05});
      break;
      
    case ',':
      //movabs rax,0    ;read
      //movabs rdi,0    ;stdin
      //mov rsi,r8      ;buffer
      //movabs rdx,1    ;count
      //syscall
      push_bytes(x64_code, {0x48,0xB8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00});
      push_bytes(x64_code, {0x48,0xBF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00});
      push_bytes(x64_code, {0x4C,0x89,0xC6});
      push_bytes(x64_code, {0x48,0xBA,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00});
      push_bytes(x64_code, {0x0F,0x05});
      break;
      
    case '[':
      //cmp BYTE PTR [r8],0
      //jz rel32
      push_bytes(x64_code, {0x41,0x80,0x38,0x00});
      push_bytes(x64_code, {0x0F,0x84,0x00,0x00,0x00,0x00});

      //push address to stack
      jump_stack.push_back(x64_code.size());
      break;
    case ']':
      {
	//cmp BYTE PTR [r8],0
	//jnz rel32
	push_bytes(x64_code, {0x41,0x80,0x38,0x00});
	push_bytes(x64_code, {0x0F,0x85,0x00,0x00,0x00,0x00});

	if (jump_stack.empty()) {
	  cout << "Mismatched '['" << endl;
	  exit(0);
	}
	uint32_t offset = x64_code.size() - jump_stack.back();
	
	fill_offset(x64_code, jump_stack.back() - 4, offset);
	fill_offset(x64_code, x64_code.size() - 4, -offset);
	
	jump_stack.pop_back();
      }
      break;
      
    default: //throws error on non-instruction char
      cout << "Illegal Instruction: " << instruction << endl;
      exit(0);
      break;
    }
  }

  if (!jump_stack.empty()) {
    cout << "Mismatched ']'" << endl;
    exit(0);
  }

  //ret
  push_bytes(x64_code, {0xc3});

  return x64_code;
}

void *allocate_executable_memory(size_t size) {
  void *ptr = mmap(0, size, PROT_READ | PROT_WRITE | PROT_EXEC,
		   MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (ptr == (void*) -1) {
    perror("mmap");
    exit(0);
  }

  return ptr;
}

int main(int argc, char **argv) {
  if (argc != 2) {
    cout << "usage: ./brainfast filename" << endl;
    return 0;
  }

  //32768 8-bit cells
  uint8_t memory[0x7FFF] = {0};

  string instructions = parse_file(argv[1]);
  vector<uint8_t> x64_code = compile_to_x64(instructions, memory);

  ofstream outfile;
  outfile.open("a.out", ios::out | ios::binary);

  for (auto byte: x64_code) {
    outfile << byte;
  }
  outfile.close();

  void (*bf_program)(void) = (void (*)(void)) allocate_executable_memory(x64_code.size());
  memcpy((void*) bf_program, (void*) x64_code.data(), x64_code.size());
  bf_program();
  
  return 0;
}
