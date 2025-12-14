cmake_minimum_required(VERSION 4.0)
project(Programm2)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static -static-libgcc -static-libstdc++")

add_executable(Programm2 main2.cpp
        funcs2.cpp
        funcs2.h)
