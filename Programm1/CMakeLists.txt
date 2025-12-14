cmake_minimum_required(VERSION 4.0)
project(Programm1)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static -static-libgcc -static-libstdc++")

add_executable(Programm1 main1.cpp
        funcs1.cpp
        funcs1.h)
