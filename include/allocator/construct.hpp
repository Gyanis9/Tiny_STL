#ifndef TINY_STL_CONSTRUCT_HPP
#define TINY_STL_CONSTRUCT_HPP
#include <new>
#include "iterator/iterator.hpp"

namespace Tiny
{
    /**
     * @brief 在给定的地址上构造类型 T1 的对象，使用传入的 value 初始化
     */
    template <typename T1, typename T2>
    void construct(T1* p, const T2& value)
    {
        new(p) T1(value); // 使用 placement new 进行对象构造
    }

    /**
     * @brief 销毁目标对象，调用其析构函数
     */
    template <typename T>
    void destroy(T* pointer)
    {
        pointer->~T();
    }

    /**
     * @brief 销毁范围内的对象，当类型没有显式析构函数时使用
     */
    template <typename ForwardIterator>
    void destroy_aux(ForwardIterator first, ForwardIterator last, false_type)
    {
        for (; first != last; ++first)
            destroy(&*first); // 对范围中的每个元素调用 destroy
    }

    /**
     * @brief 销毁范围内的对象，当类型有显式析构函数时使用
     */
    template <typename ForwardIterator>
    void destroy_aux(ForwardIterator, ForwardIterator, true_type)
    {
        // No-op for types with trivial destructors
    }

    /**
     * @brief 销毁给定范围内的所有对象，决定是否需要显式析构
     */
    template <typename ForwardIterator, typename T>
    void destroy(ForwardIterator first, ForwardIterator last, T*)
    {
        typedef typename type_traits<T>::has_trivial_destructor trivial_destructor;
        destroy_aux(first, last, trivial_destructor()); // 根据类型是否有显式析构函数选择合适的销毁方式
    }

    /**
     * @brief 销毁给定范围内的所有对象
     */
    template <typename ForwardIterator>
    void destroy(ForwardIterator first, ForwardIterator last)
    {
        return destroy(first, last, value_type(first)); // 传递范围内对象的类型
    }

    /**
     * @brief 特化销毁 char* 类型的空操作
     */
    inline void destroy(char*, char*)
    {
    }

    /**
     * @brief 特化销毁 wchar_t* 类型的空操作
     */
    inline void destroy(wchar_t*, wchar_t*)
    {
    }
}

#endif
