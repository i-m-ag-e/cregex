cmake_minimum_required(VERSION 3.31)
project(cregex C)

set(CMAKE_C_STANDARD 17)

add_library(cregex STATIC
        src/cregex.c
        include/cregex/CRegex.h
        include/cregex/AST.h
        src/AST.c
        include/cregex/Parser.h)
target_include_directories(cregex PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
add_executable(test test/main.c)
target_link_libraries(test PUBLIC cregex)