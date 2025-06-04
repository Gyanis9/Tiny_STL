#ifndef TINY_STL_ALLOCATOR_HPP
#define TINY_STL_ALLOCATOR_HPP

#include <new>
#include <cstring>
#include <algorithm>

#include "construct.hpp"
#include "../iiterator/iterator.hpp"

namespace Tiny
{
    template <int ints>
    class _malloc_alloc_template
    {
    private:
        static void* oom_malloc(size_t);

        static void* oom_realloc(void*, size_t);

        static void (*_malloc_alloc_oom_handler)();

    public:
        static void* allocate(size_t n)
        {
            void* result = malloc(n);
            if (nullptr == result)
                result = oom_malloc(n);
            return result;
        }

        static void deallocate(void* p, size_t /* n*/)
        {
            free(p);
        }

        static void* reallocate(void* p, size_t /* old_sz*/, size_t new_sz)
        {
            void* result = realloc(p, new_sz);
            if (nullptr == result)
                result = oom_realloc(p, new_sz);
            return result;
        }

        static void (* set_malloc_handler(void (*f)()))()
        {
            void (*old)() = _malloc_alloc_oom_handler;
            _malloc_alloc_oom_handler = f;
            return (old);
        }
    };

    template <int ints>
    void (*_malloc_alloc_template<ints>::_malloc_alloc_oom_handler)() = nullptr;

    template <int ints>
    void* _malloc_alloc_template<ints>::oom_malloc(size_t n)
    {
        void (*my_malloc_handler)() = nullptr;
        for (;;)
        {
            my_malloc_handler = _malloc_alloc_oom_handler;
            if (nullptr == my_malloc_handler)
                throw std::bad_alloc();
            (*my_malloc_handler)();
            if (void* result = malloc(n))
                return result;
        }
    }

    template <int ints>
    void* _malloc_alloc_template<ints>::oom_realloc(void* p, size_t n)
    {
        void (*my_alloc_handler)() = nullptr;
        for (;;)
        {
            my_alloc_handler = _malloc_alloc_oom_handler;
            if (nullptr == my_alloc_handler)
                throw std::bad_alloc();
            if (void* result = realloc(p, n))
                return result;
        }
    }

    typedef _malloc_alloc_template<0> malloc_alloc;

    /**二级配置器*/
    enum
    {
        ALIGN = 8
    };

    enum
    {
        MAX_BYTES = 128
    };

    enum
    {
        NFREELISTS = MAX_BYTES / ALIGN
    };

    template <bool threads, int ints>
    class _default_alloc_template
    {
    private:
        static size_t ROUND_UP(size_t bytes)
        {
            return (((bytes) + ALIGN - 1) & ~(ALIGN - 1));
        }

    private:
        union obj
        {
            union obj* free_list_link;
            char client_data[1];
        };

    private:
        static obj* volatile free_list[NFREELISTS];

        static size_t FREELIST_INDEX(size_t bytes)
        {
            return (((bytes) + ALIGN - 1) / ALIGN - 1);
        }

        static void* refill(size_t n);

        static char* chunk_alloc(size_t size, int& nobjs);

        static char* start_free;
        static char* end_free;
        static size_t heap_size;

    public:
        static void* allocate(size_t n);

        static void deallocate(void* p, size_t n);

        static void* reallocate(void* p, size_t old_sz, size_t new_sz);
    };

    template <bool threads, int ints>
    char* _default_alloc_template<threads, ints>::start_free = nullptr;
    template <bool threads, int ints>
    char* _default_alloc_template<threads, ints>::end_free = nullptr;
    template <bool threads, int ints>
    size_t _default_alloc_template<threads, ints>::heap_size = 0;

    template <bool threads, int ints>
    typename _default_alloc_template<threads, ints>::obj* volatile _default_alloc_template<threads, ints>::free_list[
        NFREELISTS] = {
        nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
        nullptr, nullptr, nullptr,
        nullptr
    };


    template <bool threads, int ints>
    char* _default_alloc_template<threads, ints>::chunk_alloc(size_t size, int& nobjs)
    {
        char* result;
        size_t total_bytes = size * nobjs;
        auto bytes_left = static_cast<size_t>(end_free - start_free);
        if (bytes_left >= total_bytes)
        {
            result = start_free;
            start_free += total_bytes;
            return (result);
        }
        else if (bytes_left >= size)
        {
            nobjs = bytes_left / size;
            total_bytes = nobjs * size;
            result = start_free;
            start_free += total_bytes;
            return (result);
        }
        else
        {
            /**heap_size 是已经从系统中分配的堆的大小，通过右移 4 位 (>> 4) 来计算出一个比例值，相当于 heap_size / 16
             *这是为了根据目前已分配的内存大小，动态地调整新分配的内存数量。这样可以让每次新申请的内存不仅与当前的需求相关，也与已经分配的内存量相关*/
            size_t bytes_to_get = 2 * total_bytes + ROUND_UP(heap_size >> 4);
            if (bytes_left > 0)
            {
                obj* volatile * my_free_list = free_list + FREELIST_INDEX(bytes_left);
                ((obj*)(start_free))->free_list_link = *my_free_list;
                *my_free_list = (obj*)(start_free);
            }
            start_free = (char*)(malloc(bytes_to_get));
            if (nullptr == start_free)
            {
                int i;
                obj *volatile *my_free_list, *p;

                for (i = size; i <= MAX_BYTES; i += ALIGN)
                {
                    my_free_list = free_list + FREELIST_INDEX(i);
                    p = *my_free_list;
                    if (nullptr != p)
                    {
                        *my_free_list = p->free_list_link;
                        start_free = (char*)(p);
                        end_free = start_free + i;
                        return (chunk_alloc(size, nobjs));
                    }
                }
                end_free = nullptr;
            }
            heap_size += bytes_to_get;
            end_free = start_free + bytes_to_get;
            return (chunk_alloc(size, nobjs));
        }
    }

    template <bool threads, int ints>
    void* _default_alloc_template<threads, ints>::refill(size_t n)
    {
        int nobjs = 20;
        char* chunk = chunk_alloc(n, nobjs);
        obj* volatile * my_free_list;
        obj *result, *current_obj, *next_obj;
        int i;
        if (1 == nobjs)
            return (chunk);
        my_free_list = free_list + FREELIST_INDEX(n);
        result = (obj*)chunk;
        *my_free_list = next_obj = (obj*)(chunk + n);
        for (i = 1;; ++i)
        {
            current_obj = next_obj;
            next_obj = (obj*)((char*)next_obj + n);
            if (nobjs - 1 == i)
            {
                current_obj->free_list_link = nullptr;
                break;
            }
            else
            {
                current_obj->free_list_link = next_obj;
            }
        }
        return (result);
    }


    template <bool threads, int ints>
    void _default_alloc_template<threads, ints>::deallocate(void* p, size_t n)
    {
        obj* q = static_cast<obj*>(p);
        obj* volatile * my_free_list;
        if (n > static_cast<size_t>(MAX_BYTES))
        {
            malloc_alloc::deallocate(p, n);
            return;
        }
        my_free_list = free_list + FREELIST_INDEX(n);
        q->free_list_link = *my_free_list;
        *my_free_list = q;
    }

    template <bool threads, int ints>
    void* _default_alloc_template<threads, ints>::reallocate(void* p, size_t old_sz, size_t new_sz)
    {
        if (old_sz > static_cast<size_t>(MAX_BYTES) && new_sz > static_cast<size_t>(MAX_BYTES))
            return (malloc_alloc::reallocate(p, old_sz, new_sz));
        if (ROUND_UP(old_sz) == ROUND_UP(new_sz))
            return p;
        void* result = allocate(new_sz);
        if (result != nullptr)
        {
            size_t copy_sz = new_sz > old_sz ? new_sz : old_sz;
            memcpy(result, p, copy_sz);
            deallocate(p, old_sz);
        }
        return (result);
    }

    template <bool threads, int ints>
    void* _default_alloc_template<threads, ints>::allocate(size_t n)
    {
        obj* volatile * my_free_list;
        obj* result = nullptr;
        if (n > static_cast<size_t>(MAX_BYTES))
            return (malloc_alloc::allocate(n));
        my_free_list = free_list + FREELIST_INDEX(n);
        result = *my_free_list;
        if (result == nullptr)
        {
            void* r = refill(ROUND_UP(n));
            return r;
        }
        *my_free_list = result->free_list_link;
        return (result);
    }

    typedef _default_alloc_template<0, 0> alloc;

    template <typename T, typename Alloc>
    class simple_alloc
    {
    public:
        static T* allocate(size_t n)
        {
            return 0 == n ? 0 : (T*)(Alloc::allocate(n * sizeof(T)));
        }

        static T* allocate(void)
        {
            return static_cast<T*>(Alloc::allocate(sizeof(T)));
        }

        static void deallocate(T* p, size_t n)
        {
            if (0 != n)
            {
                Alloc::deallocate(p, n * sizeof(T));
            }
        }

        static void deallocate(T* p)
        {
            Alloc::deallocate(p, sizeof(T));
        }
    };

    /**uninitialized_fill_n*/
    template <typename ForwardIterator, typename Size, typename T>
    inline ForwardIterator _uninitialized_fill_n_aux(ForwardIterator first, Size n, const T& x, _false_type)
    {
        ForwardIterator cur = first;
        for (; n > 0; n--, ++cur)
            construct(&*cur, x);
        return cur;
    }

    template <typename ForwardIterator, typename Size, typename T>
    inline ForwardIterator _uninitialized_fill_n_aux(ForwardIterator first, Size n, const T& x, _true_type)
    {
        return std::fill_n(first, n, x);
    }

    template <typename ForwardIterator, typename Size, typename T, typename T1>
    inline ForwardIterator _uninitialized_fill_n(ForwardIterator first, Size n, const T& x, T1*)
    {
        typedef typename _type_traits<T1>::is_POD_type is_POD;
        return _uninitialized_fill_n_aux(first, n, x, is_POD());
    }

    template <typename ForwardIterator, typename Size, typename T>
    inline ForwardIterator uninitialized_fill_n(ForwardIterator first, Size n, const T& x)
    {
        return _uninitialized_fill_n(first, n, x, value_type(first));
    }


    /**uninitialized_copy*/
    inline char* uninitialized_copy(const char* first, const char* last, char* result)
    {
        memmove(result, first, last - first);
        return result + (last - first);
    }

    inline wchar_t* uninitialized_copy(const wchar_t* first, const wchar_t* last, wchar_t* result)
    {
        memmove(result, first, sizeof(wchar_t) * (last - first));
        return result + (last - first);
    }

    template <typename InputIterator, typename ForwardIterator>
    inline ForwardIterator
    _uninitialized_copy_aux(InputIterator first, InputIterator last, ForwardIterator result, _false_type)
    {
        ForwardIterator cur = result;
        for (; first != last; ++first, ++cur)
        {
            construct(&*cur, *first);
        }
        return cur;
    }

    template <typename InputIterator, typename ForwardIterator>
    inline ForwardIterator
    _uninitialized_copy_aux(InputIterator first, InputIterator last, ForwardIterator result, _true_type)
    {
        return std::copy(first, last, result);
    }

    template <typename InputIterator, typename ForwardIterator, typename T>
    inline ForwardIterator _uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result, T*)
    {
        typedef typename _type_traits<T>::is_POD_type is_POD;
        return _uninitialized_copy_aux(first, last, result, is_POD());
    }

    template <typename InputIterator, typename ForwardIterator>
    inline ForwardIterator uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result)
    {
        return _uninitialized_copy(first, last, result, value_type(result));
    }


    /**uninitialized_fill*/
    template <typename ForwardIterator, typename T>
    inline void
    _uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T& x, _true_type)
    {
        std::fill(first, last, x);
    }

    template <typename ForwardIterator, typename T>
    inline void
    _uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T& x, _false_type)
    {
        ForwardIterator cur = first;
        for (; cur != last; ++cur)
            construct(&*cur, x);
    }

    template <typename ForwardIterator, typename T, typename T1>
    inline void _uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& x, T1*)
    {
        typedef typename _type_traits<T1>::is_POD_type is_POD;
        _uninitialized_fill_aux(first, last, x, is_POD());
    }

    template <typename ForwardIterator, typename T>
    inline void uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& x)
    {
        _uninitialized_fill(first, last, x, value_type(first));
    }
}


#endif //TINY_STL_ALLOCATOR_HPP
