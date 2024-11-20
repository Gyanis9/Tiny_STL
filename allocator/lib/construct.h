//
// Created by guo on 24-11-20.
//

#ifndef TINY_STL_CONSTRUCT_H
#define TINY_STL_CONSTRUCT_H

#include <new>
#include "../../iterator/lib/iterator.h"

namespace Gyanis {
    /*根据传入指针创建内存区域并将该指针指向该内存区域，内存区域存储的值为value*/
    template<typename T1, typename T2>
    inline void construct(T1 *p, const T2 &value) {
        new(p) T1(value);
    }

    template<typename T>
    inline void destroy(T *pointer) {
        pointer->~T();
    }


    template<typename ForwardIterator>
    inline void _destroy_aux(ForwardIterator first, ForwardIterator last, _false_type) {
        for (; first != last; ++first)
            destroy(&*first);
    }

    template<typename ForwardIterator>
    inline void _destroy_aux(ForwardIterator first, ForwardIterator last, _true_type) {}

    template<typename ForwardIterator, typename T>
    inline void _destroy(ForwardIterator first, ForwardIterator last, T *) {
        typedef typename _type_traits<T>::has_trivial_destructor trivial_destructor;
        _destroy_aux(first, last, trivial_destructor());
    }

    template<typename ForwardIterator>
    inline void destroy(ForwardIterator first, ForwardIterator last) {
        return _destroy(first, last, value_type(first));
    }


    inline void destroy(char *, char *) {}

    inline void destroy(wchar_t *, wchar_t *) {}

}

#endif //TINY_STL_CONSTRUCT_H
