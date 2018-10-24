CXX = g++

ASMJIT_DIR = ~/asmjit
ASMJIT_INCLUDE = -I $(ASMJIT_DIR)/src/
ASMJIT_LIB = -L $(ASMJIT_DIR)

CXX_FLAGS = -Wall -Wextra -g -Og $(ASMJIT_INCLUDE) -c
LD_FLAGS = $(ASMJIT_LIB) -lasmjit -g

EXE = brainfast

all: $(EXE)

$(EXE): main.o parser.o compiler.o
	$(CXX) $^ $(LD_FLAGS) -o $(EXE)

main.o: main.cpp compiler.h
	$(CXX) $< $(CXX_FLAGS) -o $@

parser.o: parser.cpp parser.h
	$(CXX) $< $(CXX_FLAGS) -o $@

compiler.o: compiler.cpp compiler.h
	$(CXX) $< $(CXX_FLAGS) -o $@

env:
	@echo "export LD_LIBRARY_PATH=$$LD_LIBRARY_PATH::$(ASMJIT_DIR)" >> env.sh
	@chmod u+x env.sh

test:
	@./$(EXE) tests/bitwidth.b

clean:
	@rm -rf *.o
	@rm -rf $(EXE)
