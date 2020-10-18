cmake_minimum_required(VERSION 3.6)
project(huffman C)

include_directories(include)
set(CMAKE_C_STANDARD 99)

add_executable(huffman src/main.c src/huffman.c include/huffman.h)