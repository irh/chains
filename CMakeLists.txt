cmake_minimum_required(VERSION 3.1)
project(chains)

enable_testing()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++1z")

include_directories(/usr/local/include include third-party)

set(CATCH_MAIN src/catch-main.cpp)

add_executable(experiments
  ${CATCH_MAIN}
  src/experiments/process.cpp
  src/experiments/parameter.cpp)

add_test(experiments experiments)

add_executable(chains
  ${CATCH_MAIN}
  src/chains.cpp)

add_test(chains chains)

add_executable(simple
  src/simple.cpp)

add_custom_target(ir
  clang -O3 -DNDEBUG -std=c++1z -I/usr/local/include -I../include -I../third-party -S -emit-llvm ../src/simple.cpp -o simple.ll
  DEPENDS simple)

