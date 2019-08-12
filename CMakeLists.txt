cmake_minimum_required(VERSION 3.14)
project(idcompare C)

set(CMAKE_C_STANDARD 99)

add_executable(idcompare main.c idoc.h)