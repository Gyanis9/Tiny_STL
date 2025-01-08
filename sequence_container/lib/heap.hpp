//
// Created by guo on 24-12-4.
//

#ifndef TINY_STL_HEAP_HPP
#define TINY_STL_HEAP_HPP

#include "../../allocator/lib/allocator.hpp"
#include "../../iterator/lib/iterator.hpp"

namespace Gyanis {
    template<typename RandomAccessIterator, typename Distance, typename T>
    inline void _push_heap(RandomAccessIterator first, Distance holeIndex, Distance topIndex, T value) {
        Distance parent = (holeIndex - 1) / 2;/**找出父节点*/
        while (holeIndex > topIndex && *(first + parent) < value) {/**尚未到达顶端并且父节点值小于新值*/
            *(first + holeIndex) = *(first + parent);
            holeIndex = parent;
            parent = (holeIndex - 1) / 2;
        }
        *(first + holeIndex) = value;
    }

    template<typename RandomAccessIterator, typename Distance, typename T>
    inline void _push_heap_aux(RandomAccessIterator first, RandomAccessIterator last, Distance *, T *) {
        _push_heap(first, Distance((last - first) - 1), Distance(0), static_cast<T>(*(last - 1)));
    }

    /**根据新插入节点的值进行回溯处理定位出该值正确的位置并安插*/
    template<typename RandomAccessIterator>
    inline void push_heap(RandomAccessIterator first, RandomAccessIterator last) {
        _push_heap_aux(first, last, difference_type(first), value_type(first));
    }

    template<typename RandomAccessIterator, typename T, typename Distance>
    inline void _adjust_heap(RandomAccessIterator first, Distance holeIndex, Distance len, T value) {
        Distance topIndex = holeIndex;
        Distance secondChild = 2 * holeIndex + 2;
        while (secondChild < len) {
            if (*(first + secondChild) < *(first + (secondChild - 1)))
                secondChild--;
            *(first + holeIndex) = *(first + secondChild);
            holeIndex = secondChild;
            secondChild = 2 * (secondChild + 1);
        }
        if (secondChild == len) {
            *(first + holeIndex) = *(first + (secondChild - 1));
            holeIndex = secondChild - 1;
        }
        _push_heap(first, holeIndex, topIndex, value);
    }

    template<typename RandomAccessIterator, typename T, typename Distance>
    inline void
    _pop_heap(RandomAccessIterator first, RandomAccessIterator last, RandomAccessIterator result, T value, Distance *) {
        *result = *first;/**将尾值设为首值*/
        _adjust_heap(first, Distance(0), Distance(last - first), value);
    }

    template<typename RandomAccessIterator, typename T>
    inline void _pop_heap_aux(RandomAccessIterator first, RandomAccessIterator last, T *) {
        _pop_heap(first, last - 1, last - 1, static_cast<T>(*(last - 1)), difference_type(first));
    }

    template<typename RandomAccessIterator>
    inline void pop_heap(RandomAccessIterator first, RandomAccessIterator last) {
        _pop_heap_aux(first, last, value_type(first));
    }

    template<typename RandomAccessIterator>
    void sort_heap(RandomAccessIterator first, RandomAccessIterator last) {
        while (last - first > 1) {
            pop_heap(first, last - 1);
        }
    }

    template<typename RandomAccessIterator, typename T, typename Distance>
    void _make_heap(RandomAccessIterator first, RandomAccessIterator last, T *, Distance *) {
        if (last - first < 2)
            return;
        Distance len = last - first;
        Distance parent = (len - 2) / 2;;
        while (true) {
            _adjust_heap(first, parent, len, static_cast<T>(*(first + parent)));
            if (parent == 0)
                return;
            parent--;
        }
    }

    template<typename RandomAccessIterator>
    inline void make_heap(RandomAccessIterator first, RandomAccessIterator last) {
        _make_heap(first, last, value_type(first), difference_type(first));
    }

}


#endif //TINY_STL_HEAP_HPP
