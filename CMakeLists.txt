cmake_minimum_required(VERSION 3.28)
project(Tiny_STL)

set(CMAKE_CXX_STANDARD 17)

add_executable(Tiny_STL main.cpp
        allocator/lib/allocator.hpp
        test.h
        iterator/lib/iterator.hpp
        allocator/lib/construct.hpp
        sequence_container/lib/vector.hpp
        sequence_container/lib/list.hpp
        sequence_container/lib/deque.hpp
        sequence_container/lib/stack.hpp
        sequence_container/lib/queue.hpp
        sequence_container/lib/heap.hpp)
