cmake_minimum_required(VERSION 2.8)
include (CPack)
project(kaleidoscope)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(CheckIncludeFile)
include(CheckIncludeFileCXX)
include(CheckLibraryExists)
include(CheckCXXCompilerFlag)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -g -Wall")
link_directories(/usr/local/lib/)
include_directories(include)
include_directories(~/include)

find_package(LLVM 3.7 REQUIRED)

include_directories(LLVM_INCLUDE_DIRS)

message("special note about CHECK target… ${LLVM_INCLUDE_DIRS}")
add_library(ast ast.cc)
add_library(parser parser.cc)
add_library(lexer lexer.cc)
target_link_libraries(parser lexer glog)

add_executable(topParse topParsing.cc)
target_link_libraries(topParse lexer ast parser LLVMMIRParser LLVMCore
  LLVMSupport LLVMCodeGen LLVMSelectionDAG LLVMSupport LLVMTarget LLVMAnalysis LLVMCodeGen)
