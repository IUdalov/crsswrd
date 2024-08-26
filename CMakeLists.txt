cmake_minimum_required(VERSION 3.21)
project(crsswrd)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -g -Wall") # -Werror
add_executable(crsswrd src/cpp/main.cpp src/cpp/kdtree.h)
