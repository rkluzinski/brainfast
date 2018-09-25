# brainfast

Run brainfuck at breakneck speeds using dynamic recompilation.

## Getting Started

How to use brainfast.

### Prerequisites

Any C++ compiler, GNU make, and [AsmJit](https://github.com.asmjit/asmjit).

### Installing

Build the AsmJit shared library following their [instructions](https://github.com/asmjit/asmjit#configuring--building).

Clone the brainfast github repository.

```
$ git clone https://github.com/rkluzinski/brainfast
```

Edit the variables in the Makefile.

```
CXX = g++

ASMJIT_INCLUDE = -I ~/asmjit/src/
ASMJIT_LIB = -L ~/asmjit/

CXX_FLAGS = $(ASMJIT_INCLUDE) -march=native -O2  -c
LD_FLAGS = $(ASMJIT_LIB) -lasmjit
```

Run make to compile the program.

```
$ make
```

## Running the tests

The tests are run using the makefile. It output "Hello World!", followed by the cell size.

```
$ make test
Hello World! 255
```

The test file (bitwidth.b) is from rdebath's [Brainfuck](https://github.com/rdebath/Brainfuck) repository.

## Authors

* **Ryan Kluzinski** - *Initial work* - [rkluzinski](https://github.com/rkluzinski)

## License

TODO: add MIT license.
<This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details>

## Acknowledgments

* http://www.hevanet.com/cristofd/brainfuck/
* https://github.com/rdebath/Brainfuck
* https://eli.thegreenplace.net/
* https://defuse.ca/online-x86-assembler.htm

