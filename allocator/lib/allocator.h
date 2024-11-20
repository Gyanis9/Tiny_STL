//
// Created by guo on 24-11-20.
//

#ifndef TINY_STL_ALLOCATOR_H
#define TINY_STL_ALLOCATOR_H

#include <new>

/*根据传入指针创建内存区域并将该指针指向该内存区域，内存区域存储的值为value*/
template<typename T1, typename T2>
inline void construct(T1 *p, const T2 &value) {
    new(p) T1(value);
}


#endif //TINY_STL_ALLOCATOR_H
