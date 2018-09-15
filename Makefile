CXX = gcc
EXE = brainfast
C_FLAGS = -march=native -O1 -DELF_NAME=\"$(EXE)\"

all: main.c
	$(CXX) $^ $(C_FLAGS) -o $(EXE)

test:
	@echo "Running torture test. Should output 'Hello World!' followed by the cell size."
	@./$(EXE) bitwidth.b

clean:
	@rm -rf *.o
	@rm -rf $(EXE)
