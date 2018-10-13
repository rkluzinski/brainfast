CXX = g++

ASMJIT_DIR = ~/asmjit
ASMJIT_INCLUDE = -I $(ASMJIT_DIR)/src/
ASMJIT_LIB = -L $(ASMJIT_DIR)

CXX_FLAGS = -Wall -g -Og $(ASMJIT_INCLUDE) -c
LD_FLAGS = $(ASMJIT_LIB) -lasmjit -g

EXE = brainfast

all: $(EXE)

$(EXE): main.o compiler.o optimize.o assembler.o
	$(CXX) $^ $(LD_FLAGS) -o $(EXE)

main.o: main.cpp
	$(CXX) $< $(CXX_FLAGS) -o $@

compiler.o: compiler.cpp
	$(CXX) $< $(CXX_FLAGS) -o $@

optimize.o: optimize.cpp
	$(CXX) $< $(CXX_FLAGS) -o $@

assembler.o: assembler.cpp
	$(CXX) $< $(CXX_FLAGS) -o $@

env:
	@echo "export LD_LIBRARY_PATH=$$LD_LIBRARY_PATH::$(ASMJIT_DIR)" >> env.sh
	@chmod u+x env.sh

test:
	@./$(EXE) tests/bitwidth.b

clean:
	@rm -rf *.o
	@rm -rf $(EXE)
