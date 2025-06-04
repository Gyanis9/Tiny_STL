//
// Created by guo on 24-11-20.
//



#ifndef TINY_STL_ITERATOR_HPP
#define TINY_STL_ITERATOR_HPP

#include <cstddef>

namespace Tiny {
    struct input_iterator_tag {
    };
    struct output_iterator_tag {
    };
    struct forward_iterator_tag : public input_iterator_tag {
    };
    struct bidirectional_iterator_tag : public forward_iterator_tag, public std::input_iterator_tag {
    };
    struct random_access_iterator_tag : public bidirectional_iterator_tag {
    };

    template<typename Category, typename T, typename Distance=ptrdiff_t, typename Pointer=T *, typename Reference=T &>
    struct iterator {
        typedef Category iterator_category;
        typedef T value_type;
        typedef Distance difference_type;
        typedef Pointer pointer;
        typedef Reference reference;
    };

    template<typename Iterator>
    struct iterator_traits {
        typedef typename Iterator::iterator_category iterator_category;
        typedef typename Iterator::value_type value_type;
        typedef typename Iterator::difference_type difference_type;
        typedef typename Iterator::pointer pointer;
        typedef typename Iterator::reference reference;
    };

    /**针对原生指针进行偏特化*/
    template<typename T>
    struct iterator_traits<T *> {
        typedef random_access_iterator_tag iterator_category;
        typedef T value_type;
        typedef ptrdiff_t difference_type;
        typedef T *pointer;
        typedef T &reference;
    };
    /**对const原生指针进行偏特化*/
    template<typename T>
    struct iterator_traits<const T *> {
        typedef random_access_iterator_tag iterator_category;
        typedef T value_type;
        typedef ptrdiff_t difference_type;
        typedef T *pointer;
        typedef T &reference;
    };

    /**获取迭代器的类型*/
    template<typename Iterator>
    inline typename iterator_traits<Iterator>::iterator_category iterator_category(const Iterator &) {
        typedef typename iterator_traits<Iterator>::iterator_category category;
        return category();
    }

    /**获取迭代器的difference type*/
    template<typename Iterator>
    inline typename iterator_traits<Iterator>::difference_type *difference_type(const Iterator &) {
        return static_cast<typename iterator_traits<Iterator>::difference_type *> (0);
    }

    /**获取迭代器的value type
    返回指针而不是实际值可以简化实现，避免不必要的构造开销，同时满足类型信息提取的需求*/
    template<typename Iterator>
    inline typename iterator_traits<Iterator>::value_type *value_type(const Iterator &) {
        return static_cast<typename iterator_traits<Iterator>::value_type *> (0);
    }

    /**以下是整组distance函数*/
    template<typename InputIterator>
    inline typename iterator_traits<InputIterator>::difference_type
    _distance(InputIterator first, InputIterator last, input_iterator_tag) {
        typename iterator_traits<InputIterator>::difference_type n = 0;
        while (first != last) {
            ++n;
            ++first;
        }
        return n;
    }

    template<typename RandomAccessIterator>
    inline typename iterator_traits<RandomAccessIterator>::difference_type
    _distance(RandomAccessIterator first, RandomAccessIterator last, random_access_iterator_tag) {
        return last - first;
    }


    template<typename InputIterator>
    inline typename iterator_traits<InputIterator>::difference_type distance(InputIterator first, InputIterator last) {
        typedef typename iterator_traits<InputIterator>::iterator_category category;
        return _distance(first, last, category());
    }

    /**以下是整组advance函数*/
    template<typename InputIterator, typename Distance>
    inline void _advance(InputIterator &i, Distance n, input_iterator_tag) {
        while (n--)
            i++;
    }

    template<typename BidirectionalIterator, typename Distance>
    inline void _advance(BidirectionalIterator &i, Distance n, bidirectional_iterator_tag) {
        if (n >= 0)
            while (n--)
                i++;
        else
            while (n++)
                --i;
    }

    template<typename RandomAccessIterator, typename Distance>
    inline void _advance(RandomAccessIterator &i, Distance n, random_access_iterator_tag) {
        i += n;
    }


    template<typename InputIterator, typename Distance>
    inline void advance(InputIterator &i, Distance n) {
        _advance(i, n, iterator_category(i));
    }

    /**另一种实现方式*/
    /**    template<typename InputIterator, typename Distance>
    inline void advance(InputIterator &i, Distance n) {
        typedef typename iterator_traits<InputIterator>::iterator_category category;
        _advance(i, n, category());
    }*/

    struct _true_type {
    };
    struct _false_type {
    };

    template<typename type>
    struct _type_traits {
        typedef _true_type this_dummy_member_must_be_first;
        typedef _false_type has_trivial_default_constructor;
        typedef _false_type has_trivial_copy_constructor;
        typedef _false_type has_trivial_assignment_constructor;
        typedef _false_type has_trivial_destructor;
        typedef _false_type is_POD_type;
    };
    template<>
    struct _type_traits<char> {
        typedef _true_type this_dummy_member_must_be_first;
        typedef _true_type has_trivial_default_constructor;
        typedef _true_type has_trivial_copy_constructor;
        typedef _true_type has_trivial_assignment_constructor;
        typedef _true_type has_trivial_destructor;
        typedef _true_type is_POD_type;
    };
    template<>
    struct _type_traits<signed char> {
        typedef _true_type this_dummy_member_must_be_first;
        typedef _true_type has_trivial_default_constructor;
        typedef _true_type has_trivial_copy_constructor;
        typedef _true_type has_trivial_assignment_constructor;
        typedef _true_type has_trivial_destructor;
        typedef _true_type is_POD_type;
    };
    template<>
    struct _type_traits<unsigned char> {
        typedef _true_type this_dummy_member_must_be_first;
        typedef _true_type has_trivial_default_constructor;
        typedef _true_type has_trivial_copy_constructor;
        typedef _true_type has_trivial_assignment_constructor;
        typedef _true_type has_trivial_destructor;
        typedef _true_type is_POD_type;
    };
    template<>
    struct _type_traits<short> {
        typedef _true_type this_dummy_member_must_be_first;
        typedef _true_type has_trivial_default_constructor;
        typedef _true_type has_trivial_copy_constructor;
        typedef _true_type has_trivial_assignment_constructor;
        typedef _true_type has_trivial_destructor;
        typedef _true_type is_POD_type;
    };
    template<>
    struct _type_traits<unsigned short> {
        typedef _true_type this_dummy_member_must_be_first;
        typedef _true_type has_trivial_default_constructor;
        typedef _true_type has_trivial_copy_constructor;
        typedef _true_type has_trivial_assignment_constructor;
        typedef _true_type has_trivial_destructor;
        typedef _true_type is_POD_type;
    };
    template<>
    struct _type_traits<int> {
        typedef _true_type this_dummy_member_must_be_first;
        typedef _true_type has_trivial_default_constructor;
        typedef _true_type has_trivial_copy_constructor;
        typedef _true_type has_trivial_assignment_constructor;
        typedef _true_type has_trivial_destructor;
        typedef _true_type is_POD_type;
    };
    template<>
    struct _type_traits<unsigned int> {
        typedef _true_type this_dummy_member_must_be_first;
        typedef _true_type has_trivial_default_constructor;
        typedef _true_type has_trivial_copy_constructor;
        typedef _true_type has_trivial_assignment_constructor;
        typedef _true_type has_trivial_destructor;
        typedef _true_type is_POD_type;
    };
    template<>
    struct _type_traits<long> {
        typedef _true_type this_dummy_member_must_be_first;
        typedef _true_type has_trivial_default_constructor;
        typedef _true_type has_trivial_copy_constructor;
        typedef _true_type has_trivial_assignment_constructor;
        typedef _true_type has_trivial_destructor;
        typedef _true_type is_POD_type;
    };
    template<>
    struct _type_traits<unsigned long> {
        typedef _true_type this_dummy_member_must_be_first;
        typedef _true_type has_trivial_default_constructor;
        typedef _true_type has_trivial_copy_constructor;
        typedef _true_type has_trivial_assignment_constructor;
        typedef _true_type has_trivial_destructor;
        typedef _true_type is_POD_type;
    };
    template<>
    struct _type_traits<float> {
        typedef _true_type this_dummy_member_must_be_first;
        typedef _true_type has_trivial_default_constructor;
        typedef _true_type has_trivial_copy_constructor;
        typedef _true_type has_trivial_assignment_constructor;
        typedef _true_type has_trivial_destructor;
        typedef _true_type is_POD_type;
    };

    template<>
    struct _type_traits<double> {
        typedef _true_type this_dummy_member_must_be_first;
        typedef _true_type has_trivial_default_constructor;
        typedef _true_type has_trivial_copy_constructor;
        typedef _true_type has_trivial_assignment_constructor;
        typedef _true_type has_trivial_destructor;
        typedef _true_type is_POD_type;
    };
    template<>
    struct _type_traits<long double> {
        typedef _true_type this_dummy_member_must_be_first;
        typedef _true_type has_trivial_default_constructor;
        typedef _true_type has_trivial_copy_constructor;
        typedef _true_type has_trivial_assignment_constructor;
        typedef _true_type has_trivial_destructor;
        typedef _true_type is_POD_type;
    };
    template<typename T>
    struct _type_traits<T *> {
        typedef _true_type this_dummy_member_must_be_first;
        typedef _true_type has_trivial_default_constructor;
        typedef _true_type has_trivial_copy_constructor;
        typedef _true_type has_trivial_assignment_constructor;
        typedef _true_type has_trivial_destructor;
        typedef _true_type is_POD_type;
    };


}

#endif //TINY_STL_ITERATOR_HPP
