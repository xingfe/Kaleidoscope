cmake_minimum_required(VERSION 3.0.2)
set (CMAKE_CXX_STANDARD 14)
add_library(lexer lexer.cc)


add_library(ast ast.cc )
add_library(parser parser.cc)
target_link_libraries(parser lexer glog)

add_executable(topParse topParsing.cc)
target_link_libraries(topParse lexer ast parser )
