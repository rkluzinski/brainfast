# Brainfast

Brainfast is a brainf\*ck interpreter capable of running programs very quickly. The brainf\*ck source is optimized and recompiled to native x86-64 machine code using asmjit.

## Getting Started

Building, testing and benchmarking brainfast.

### Prerequisites

* Any C++ Compiler Supported by CMake.
* CMake Version 3.10.2 or greater.
* Python3.x (for automating tests and benchmarks).

### Installing

Clone the brainfast github repository.

```
$ git clone https://github.com/rkluzinski/brainfast
$ cd brainfast
```

Create the build directory.

```
$ mkdir build
$ cd build
```

Run CMake.

```
$ cmake ..
```

Run the generated Makefile

```
$ make
```

The executable 'bf' should be created in the build directory. Running it with no arguments should give a usage message.

```
$ ./bf
usage: ./bf [-m MEMORY_SIZE] filename
```

## Running the tests and benchmarks.

The tests and benchmarks can be run from the top-level directory.

```
$ cd ..
```

The testing program is contained in the test/ directory. It can be run automatically using python3.
The testing program bitwidth.b is from rdebath's [brainf\*ck](https://github.com/rdebath/Brainfuck) repository. It can be run with Python3 and should display 'Hello World! 255'.

```
$ python3 test/
Running bitwidth.b: Hello World! 255
```

The benchmarks are run in similar manner as the tests. The real, user and system time will be displayed for each program and the totals. The files used for benchmarking are taken from matslina's [brainf\*ck](https://github.com/matslina/bfoptimization) repository.

Programs that ouput lots to STDOUT will run faster in the benchmark. This is because the output is being piped to a file and does not have to be displayed in the terminal.

Shown below is output from a Lubuntu 18.04 VM running on a Windows 10 Host with an Intel(R) Core(TM) i5-8250U CPU.

```
$ python3 benchmarks/
program                  real     user      sys
------------------------------------------------
awib-0.4                0.046    0.032    0.004
dbfi                    0.850    0.842    0.000
factor                  0.773    0.762    0.000
hanoi                   0.060    0.055    0.000
long                    0.225    0.220    0.000
mandelbrot              1.098    1.090    0.000
total                   3.050    3.002    0.004
```

## Built With

* [AsmJit](https://github.com/asmjit/asmjit) - A remote assembler for C++
* [CMake](https://cmake.org/) - A build automation tool

## Authors

* **Ryan Kluzinski** - *Initial work* - [rkluzinski](https://github.com/rkluzinski)

## License

<This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details>

## Acknowledgments

* http://www.hevanet.com/cristofd/brainfuck/ - A great introduction to brainf\*ck.
* https://github.com/rdebath/Brainfuck - A very comprehensive test program.
* https://github.com/matslina/bfoptimization - A collection of benchmarks.
* http://calmerthanyouare.org/2015/01/07/optimizing-brainfuck.html - Various optimization strategies.
* https://eli.thegreenplace.net/ - Other work in using JIT compilation with brainf\*ck.
* http://www.nynaeve.net/?p=64 - Assembly optimization techniques.

