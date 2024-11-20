cmake_minimum_required(VERSION 3.28)
project(Tiny_STL)

set(CMAKE_CXX_STANDARD 17)

add_executable(Tiny_STL main.cpp
        allocator/src/allocator.cpp
        allocator/lib/allocator.h
        test.h
        iterator/lib/iterator.h)
