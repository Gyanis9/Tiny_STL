#ifndef TINY_STL_ITERATOR_HPP
#define TINY_STL_ITERATOR_HPP
#include <cstddef>

namespace Tiny
{
    /// 输入迭代器标签
    struct input_iterator_tag
    {
    };

    /// 输出迭代器标签
    struct output_iterator_tag
    {
    };

    /// 前向迭代器标签，继承自 input_iterator_tag
    struct forward_iterator_tag : input_iterator_tag
    {
    };

    /// 双向迭代器标签，继承自 forward_iterator_tag 和 input_iterator_tag
    struct bidirectional_iterator_tag : forward_iterator_tag, std::input_iterator_tag
    {
    };

    /// 随机访问迭代器标签，继承自 bidirectional_iterator_tag
    struct random_access_iterator_tag : bidirectional_iterator_tag
    {
    };

    /**
     * @brief 通用迭代器结构体，用于定义迭代器的基本类型
     */
    template <typename Category, typename T, typename Distance=ptrdiff_t, typename Pointer=T*, typename Reference=T&>
    struct iterator
    {
        typedef Category iterator_category; ///< 迭代器类别
        typedef T value_type; ///< 迭代器指向的数据类型
        typedef Distance difference_type; ///< 迭代器的差值类型
        typedef Pointer pointer; ///< 迭代器指向的数据类型的指针
        typedef Reference reference; ///< 迭代器引用的数据类型
    };

    /**
     * @brief 迭代器特性结构体，用于获取给定迭代器类型的类别、值类型、差值类型、指针和引用类型
     */
    template <typename Iterator>
    struct iterator_traits
    {
        typedef typename Iterator::iterator_category iterator_category;
        typedef typename Iterator::value_type value_type;
        typedef typename Iterator::difference_type difference_type;
        typedef typename Iterator::pointer pointer;
        typedef typename Iterator::reference reference;
    };

    /** 针对原生指针进行偏特化 */
    template <typename T>
    struct iterator_traits<T*>
    {
        typedef random_access_iterator_tag iterator_category;
        typedef T value_type;
        typedef ptrdiff_t difference_type;
        typedef T* pointer;
        typedef T& reference;
    };

    /** 对const原生指针进行偏特化 */
    template <typename T>
    struct iterator_traits<const T*>
    {
        typedef random_access_iterator_tag iterator_category;
        typedef T value_type;
        typedef ptrdiff_t difference_type;
        typedef const T* pointer;
        typedef const T& reference;
    };

    /**
     * @brief 获取迭代器的迭代器类别
     */
    template <typename Iterator>
    typename iterator_traits<Iterator>::iterator_category iterator_category(const Iterator&)
    {
        typedef typename iterator_traits<Iterator>::iterator_category category;
        return category();
    }

    /**
     * @brief 获取迭代器的差值类型
     */
    template <typename Iterator>
    typename iterator_traits<Iterator>::difference_type* difference_type(const Iterator&)
    {
        return static_cast<typename iterator_traits<Iterator>::difference_type*>(nullptr);
    }

    /**
     * @brief 获取迭代器的值类型
     */
    template <typename Iterator>
    typename iterator_traits<Iterator>::value_type* value_type(const Iterator&)
    {
        return static_cast<typename iterator_traits<Iterator>::value_type*>(nullptr);
    }

    /**
     * @brief 计算输入迭代器之间的距离
     */
    template <typename InputIterator>
    typename iterator_traits<InputIterator>::difference_type
    distance_(InputIterator first, InputIterator last, input_iterator_tag)
    {
        typename iterator_traits<InputIterator>::difference_type n = 0;
        while (first != last)
        {
            ++n;
            ++first;
        }
        return n;
    }

    /**
     * @brief 计算前向迭代器之间的距离
     */
    template <typename ForwardIterator>
    typename iterator_traits<ForwardIterator>::difference_type
    distance_(ForwardIterator first, ForwardIterator last, forward_iterator_tag)
    {
        return distance_(first, last, input_iterator_tag());
    }

    /**
     * @brief 计算双向迭代器之间的距离
     */
    template <typename BidirectionalIterator>
    typename iterator_traits<BidirectionalIterator>::difference_type
    distance_(BidirectionalIterator first, BidirectionalIterator last, bidirectional_iterator_tag)
    {
        return distance_(first, last, input_iterator_tag());
    }

    /**
     * @brief 计算随机访问迭代器之间的距离
     */
    template <typename RandomAccessIterator>
    typename iterator_traits<RandomAccessIterator>::difference_type
    distance_(RandomAccessIterator first, RandomAccessIterator last, random_access_iterator_tag)
    {
        return last - first;
    }

    /**
     * @brief 计算任意类型迭代器之间的距离
     */
    template <typename InputIterator>
    typename iterator_traits<InputIterator>::difference_type distance(InputIterator first, InputIterator last)
    {
        typedef typename iterator_traits<InputIterator>::iterator_category category;
        return distance_(first, last, category());
    }

    /**
     * @brief 在输入迭代器上前进指定的距离
     */
    template <typename InputIterator, typename Distance>
    void advance_(InputIterator& i, Distance n, input_iterator_tag)
    {
        while (n > 0)
        {
            --n;
            ++i;
        }
    }

    /**
     * @brief 在前向迭代器上前进指定的距离
     */
    template <typename ForwardIterator, typename Distance>
    void advance_(ForwardIterator& i, Distance n, forward_iterator_tag)
    {
        advance_(i, n, input_iterator_tag());
    }

    /**
     * @brief 在双向迭代器上前进指定的距离
     */
    template <typename BidirectionalIterator, typename Distance>
    void advance_(BidirectionalIterator& i, Distance n, bidirectional_iterator_tag)
    {
        if (n > 0)
        {
            while (n--)
                ++i;
        }
        else
        {
            while (n++)
                --i;
        }
    }

    /**
     * @brief 在随机访问迭代器上前进指定的距离
     */
    template <typename RandomAccessIterator, typename Distance>
    void advance_(RandomAccessIterator& i, Distance n, random_access_iterator_tag)
    {
        i += n;
    }

    /**
     * @brief 使迭代器前进指定的距离
     */
    template <typename InputIterator, typename Distance>
    void advance(InputIterator& i, Distance n)
    {
        advance_(i, n, iterator_category(i));
    }

    ///< 类型判断结构体
    struct true_type
    {
    };

    struct false_type
    {
    };

    /**
     * @brief 类型特征结构体，用于判断类型是否具有某些特性
     */
    template <typename type>
    struct type_traits
    {
        typedef true_type this_dummy_member_must_be_first;
        typedef false_type has_trivial_default_constructor;
        typedef false_type has_trivial_copy_constructor;
        typedef false_type has_trivial_assignment_constructor;
        typedef false_type has_trivial_destructor;
        typedef false_type is_POD_type;
    };

    /** 对基础数据类型进行偏特化 */
    template <typename T>
    struct type_traits<T*>
    {
        typedef true_type this_dummy_member_must_be_first;
        typedef true_type has_trivial_default_constructor;
        typedef true_type has_trivial_copy_constructor;
        typedef true_type has_trivial_assignment_constructor;
        typedef true_type has_trivial_destructor;
        typedef true_type is_POD_type;
    };

    ///< 对不同的基础数据类型进行特化
    template <>
    struct type_traits<char>
    {
        typedef true_type has_trivial_default_constructor;
        typedef true_type has_trivial_copy_constructor;
        typedef true_type has_trivial_assignment_constructor;
        typedef true_type has_trivial_destructor;
        typedef true_type is_POD_type;
    };

    template <>
    struct type_traits<signed char>
    {
        typedef true_type has_trivial_default_constructor;
        typedef true_type has_trivial_copy_constructor;
        typedef true_type has_trivial_assignment_constructor;
        typedef true_type has_trivial_destructor;
        typedef true_type is_POD_type;
    };

    template <>
    struct type_traits<unsigned char>
    {
        typedef true_type has_trivial_default_constructor;
        typedef true_type has_trivial_copy_constructor;
        typedef true_type has_trivial_assignment_constructor;
        typedef true_type has_trivial_destructor;
        typedef true_type is_POD_type;
    };

    template <>
    struct type_traits<short>
    {
        typedef true_type has_trivial_default_constructor;
        typedef true_type has_trivial_copy_constructor;
        typedef true_type has_trivial_assignment_constructor;
        typedef true_type has_trivial_destructor;
        typedef true_type is_POD_type;
    };

    template <>
    struct type_traits<unsigned short>
    {
        typedef true_type has_trivial_default_constructor;
        typedef true_type has_trivial_copy_constructor;
        typedef true_type has_trivial_assignment_constructor;
        typedef true_type has_trivial_destructor;
        typedef true_type is_POD_type;
    };

    template <>
    struct type_traits<int>
    {
        typedef true_type has_trivial_default_constructor;
        typedef true_type has_trivial_copy_constructor;
        typedef true_type has_trivial_assignment_constructor;
        typedef true_type has_trivial_destructor;
        typedef true_type is_POD_type;
    };

    template <>
    struct type_traits<unsigned int>
    {
        typedef true_type has_trivial_default_constructor;
        typedef true_type has_trivial_copy_constructor;
        typedef true_type has_trivial_assignment_constructor;
        typedef true_type has_trivial_destructor;
        typedef true_type is_POD_type;
    };

    template <>
    struct type_traits<long>
    {
        typedef true_type has_trivial_default_constructor;
        typedef true_type has_trivial_copy_constructor;
        typedef true_type has_trivial_assignment_constructor;
        typedef true_type has_trivial_destructor;
        typedef true_type is_POD_type;
    };

    template <>
    struct type_traits<unsigned long>
    {
        typedef true_type has_trivial_default_constructor;
        typedef true_type has_trivial_copy_constructor;
        typedef true_type has_trivial_assignment_constructor;
        typedef true_type has_trivial_destructor;
        typedef true_type is_POD_type;
    };

    template <>
    struct type_traits<float>
    {
        typedef true_type has_trivial_default_constructor;
        typedef true_type has_trivial_copy_constructor;
        typedef true_type has_trivial_assignment_constructor;
        typedef true_type has_trivial_destructor;
        typedef true_type is_POD_type;
    };

    template <>
    struct type_traits<double>
    {
        typedef true_type has_trivial_default_constructor;
        typedef true_type has_trivial_copy_constructor;
        typedef true_type has_trivial_assignment_constructor;
        typedef true_type has_trivial_destructor;
        typedef true_type is_POD_type;
    };

    template <>
    struct type_traits<long double>
    {
        typedef true_type has_trivial_default_constructor;
        typedef true_type has_trivial_copy_constructor;
        typedef true_type has_trivial_assignment_constructor;
        typedef true_type has_trivial_destructor;
        typedef true_type is_POD_type;
    };
}

#endif
