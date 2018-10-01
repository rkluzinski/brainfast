CXX = g++

ASMJIT_DIR = /home/ryan-desktop/asmjit
ASMJIT_INCLUDE = -I $(ASMJIT_DIR)/src/
ASMJIT_LIB = -L $(ASMJIT_DIR)

CXX_FLAGS = -Wall -g -Og $(ASMJIT_INCLUDE) -c
LD_FLAGS = $(ASMJIT_LIB) -lasmjit -g

EXE = brainfast

all: $(EXE) brainfast_env

$(EXE): main.o parser.o compiler.o
	$(CXX) $^ $(LD_FLAGS) -o $(EXE)

main.o: main.cpp parser.h compiler.h
	$(CXX) $< $(CXX_FLAGS) -o $@

parser.o: parser.cpp parser.h
	$(CXX) $< $(CXX_FLAGS) -o $@

compiler.o: compiler.cpp compiler.h
	$(CXX) $< $(CXX_FLAGS) -o $@

brainfast_env:
	@echo "export LD_LIBRARY_PATH=$$LD_LIBRARY_PATH::$(ASMJIT_DIR)" > brainfast_env.sh
	@chmod u+x brainfast_env.sh

test:
	@echo "Running torture test. Should output 'Hello World!' followed by the cell size."
	@./$(EXE) bitwidth.b

clean:
	@rm -rf brainfast_env.sh
	@rm -rf *.o
	@rm -rf $(EXE)
