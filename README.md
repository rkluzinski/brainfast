# Brainfast

A JIT optimizing brainfuck interpreter. Brainfuck code will be compiled into bytecode in memory, which will allow for more efficient instructions. This bytecode will then be ran by the interpreter.

## Getting Started

How to use brainfast.

### Prerequisites

Any C compiler.

### Installing

Clone the brainfast github repository.

```
$ git clone https://github.com/rkluzinski/brainfast
```

Edit the variables in the Makefile

```
CXX = gcc
C_FLAGS = -march=native -O1
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

## Authors

* **Ryan Kluzinski** - *Initial work* - [rkluzinski](https://github.com/rkluzinski)

## License

TODO: add MIT license.
<This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details>

## Acknowledgments

* http://www.hevanet.com/cristofd/brainfuck/
* https://github.com/rdebath/Brainfuck

