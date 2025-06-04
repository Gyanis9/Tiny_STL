cmake_minimum_required(VERSION 3.20)
project(Tiny_STL)

set(CMAKE_CXX_STANDARD 17)

add_executable(Tiny_STL main.cpp
        include/allocator/allocator.hpp
        test/test.h
        include/iiterator/iterator.hpp
        include/allocator/construct.hpp
        include/container/vector.hpp
        include/container/list.hpp
        include/container/deque.hpp
        include/container/stack.hpp
        include/container/queue.hpp
        include/container/heap.hpp
        include/tiny_stl.h
        src/string/string.cpp
        include/string/string.h)
