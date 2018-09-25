CXX = g++

ASMJIT_INCLUDE = -I ~/asmjit/src/
ASMJIT_LIB = -L ~/asmjit/

CXX_FLAGS = -march=native -O2 $(ASMJIT_INCLUDE) -c
LD_FLAGS = $(ASMJIT_LIB) -lasmjit

EXE = brainfast

all: $(EXE)

$(EXE): main.o
	$(CXX) $^ $(LD_FLAGS) -o $(EXE)

main.o: main.cpp
	$(CXX) $< $(CXX_FLAGS) -o $@

test:
	@echo "Running torture test. Should output 'Hello World!' followed by the cell size."
	@./$(EXE) bitwidth.b

clean:
	@rm -rf *.o
	@rm -rf $(EXE)
