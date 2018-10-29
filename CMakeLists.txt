cmake_minimum_required(VERSION 3.1)
project(Brainfast)

# project build type
set(CMAKE_BUILD_TYPE Debug)

# asmjit compilation flags
set(ASMJIT_EMBED TRUE)
set(ASMJIT_BUILD_X86 TRUE)
set(ASMJIT_DISABLE_BUILDER TRUE)
set(ASMJIT_DISABLE_COMPILER TRUE)
set(ASMJIT_DISABLE_LOGGING TRUE)
set(ASMJIT_DISABLE_TEXT TRUE)
set(ASMJIT_DISABLE_VALIDATION TRUE)

# asmjit build directory
set(ASMJIT_DIR "asmjit")
include("${ASMJIT_DIR}/CMakeLists.txt")
include_directories("${ASMJIT_DIR}/src")

add_executable(bf
  main.cpp
  parser.cpp
  compiler.cpp
  ${ASMJIT_SRC})

target_link_libraries(bf ${ASMJIT_DEPS})
