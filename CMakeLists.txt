cmake_minimum_required(VERSION 3.4)
project(LessCompiler)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c99")

set(SOURCE_FILES main.c calculate.c calculate.h util.c util.h)
add_executable(LessCompiler ${SOURCE_FILES})