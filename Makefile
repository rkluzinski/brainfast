CXX = g++
C_FLAGS = -march=native -O2

EXE = brainfast

all: main.cpp
	$(CXX) $^ $(C_FLAGS) -o $(EXE)

test:
	@echo "Running torture test. Should output 'Hello World!' followed by the cell size."
	@./$(EXE) bitwidth.b

clean:
	@rm -rf *.o
	@rm -rf $(EXE)
