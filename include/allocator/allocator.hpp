#ifndef TINY_STL_ALLOCATOR_HPP
#define TINY_STL_ALLOCATOR_HPP
#include <cstring>
#include <algorithm>
#include <functional>

#include "construct.hpp"
#include "iterator/iterator.hpp"

namespace Tiny
{
    /**
     * @brief 模板类 malloc_alloc_template，用于管理内存分配
     */
    template <int>
    class malloc_alloc_template
    {
    private:
        /**
         * @brief 内存分配失败处理：当 malloc 失败时调用该函数
         */
        static void* oom_malloc(size_t);

        /**
         * @brief 内存分配失败处理：当 realloc 失败时调用该函数
         */
        static void* oom_realloc(void*, size_t);

        /**
         * @brief 自定义内存分配失败处理函数
         */
        static std::function<void()> malloc_alloc_oom_handler;

    public:
        /**
         * @brief 分配内存
         */
        static void* allocate(const size_t n)
        {
            void* result = malloc(n);
            if (nullptr == result)
                result = oom_malloc(n);
            return result;
        }

        /**
         * @brief 释放内存
         */
        static void deallocate(void* p, size_t /** n */)
        {
            free(p);
        }

        /**
         * @brief 重新分配内存
         */
        static void* reallocate(void* p, size_t /** old_sz */, const size_t new_sz)
        {
            void* result = realloc(p, new_sz);
            if (nullptr == result)
                result = oom_realloc(p, new_sz);
            return result;
        }

        /**
         * @brief 设置自定义内存分配失败处理函数
         */
        static std::function<void()> set_malloc_handler(std::function<void()> f)
        {
            std::function<void()> old = malloc_alloc_oom_handler; // 保存旧的处理函数
            malloc_alloc_oom_handler = std::move(f); // 设置新的处理函数
            return old; // 返回旧的处理函数
        }
    };

    ///< 初始化静态成员变量 malloc_alloc_oom_handler
    template <int Align>
    std::function<void()> malloc_alloc_template<Align>::malloc_alloc_oom_handler = nullptr;

    /**
     * @brief 自定义的 malloc 失败处理函数
     */
    template <int ints>
    void* malloc_alloc_template<ints>::oom_malloc(const size_t n)
    {
        std::function<void()> my_malloc_handler = nullptr;
        for (;;)
        {
            my_malloc_handler = malloc_alloc_oom_handler;
            // 如果没有设置处理函数，抛出 std::bad_alloc 异常
            if (nullptr == my_malloc_handler)
                throw std::bad_alloc();
            // 调用当前的内存分配失败处理函数
            my_malloc_handler();
            // 尝试重新分配内存
            if (void* result = malloc(n))
            {
                return result;
            }
        }
    }

    /**
     * @brief 自定义的 realloc 失败处理函数
     */
    template <int ints>
    void* malloc_alloc_template<ints>::oom_realloc(void* p, const size_t n)
    {
        std::function<void()> my_alloc_handler = nullptr;
        for (;;)
        {
            my_alloc_handler = malloc_alloc_oom_handler;
            // 如果没有设置处理函数，抛出 std::bad_alloc 异常
            if (nullptr == my_alloc_handler)
                throw std::bad_alloc();
            // 尝试重新分配内存
            if (void* result = realloc(p, n))
                return result;
        }
    }

    ///< 定义一个 malloc_alloc 类型，使用默认 Align 为 0
    using malloc_alloc = malloc_alloc_template<0>;

    enum
    {
        ALIGN = 8 ///< 对齐边界，通常为8字节对齐
    };

    enum
    {
        MAX_BYTES = 128 ///< 内存池中每个内存块的最大字节数
    };

    enum
    {
        NFREELISTS = MAX_BYTES / ALIGN ///< 根据最大内存块大小和对齐边界，计算出自由链表的数量
    };

    /**
     * @brief 默认分配器模板类、二级配置器
     */
    template <bool threads, int ints>
    class default_alloc_template
    {
    private:
        /**
         * @brief 对齐到最近的对齐边界
         */
        static size_t ROUND_UP(const size_t bytes)
        {
            return (bytes + ALIGN - 1) & ~(ALIGN - 1);
        }

        /**
         * @brief 计算内存块的自由链表索引
         */
        static size_t FREELIST_INDEX(const size_t bytes)
        {
            return (bytes + ALIGN - 1) / ALIGN - 1;
        }

        /**
         * @brief 填充内存池
         */
        static void* refill(size_t n);

        /**
         * @brief 内存块分配
         */
        static char* chunk_alloc(size_t size, int& nobjs);

    private:
        /**
         * @brief 内存块结构体
         */
        union obj
        {
            obj* free_list_link; ///< 指向下一个空闲内存块
            char client_data[1]; ///< 存储实际分配的内存数据
        };

    private:
        static obj* volatile free_list[NFREELISTS]; ///<自由链表数组
        static char* start_free; ///<内存池的起始地址
        static char* end_free; ///< 内存池的结束地址
        static size_t heap_size; ///<堆内存的大小

    public:
        /**
         * @brief 分配内存
         */
        static void* allocate(size_t n);

        /**
         * @brief 释放内存
         */
        static void deallocate(void* p, size_t n);

        /**
         * @brief 重新分配内存
         */
        static void* reallocate(void* p, size_t old_sz, size_t new_sz);
    };

    template <bool threads, int ints>
    char* default_alloc_template<threads, ints>::start_free = nullptr; ///< 初始化内存池的起始地址，初始值为 nullptr

    template <bool threads, int ints>
    char* default_alloc_template<threads, ints>::end_free = nullptr; ///< 内存池的结束地址

    template <bool threads, int ints>
    size_t default_alloc_template<threads, ints>::heap_size = 0; ///< 内存池的总大小

    template <bool threads, int ints>
    typename default_alloc_template<threads, ints>::obj* volatile default_alloc_template<threads, ints>::free_list[
        NFREELISTS] = {
        nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
        nullptr, nullptr, nullptr, nullptr
    }; ///<自由链表数组


    template <bool threads, int ints>
    char* default_alloc_template<threads, ints>::chunk_alloc(const size_t size, int& nobjs)
    {
        char* result; // 结果指针，指向分配的内存块
        size_t total_bytes = size * nobjs; // 计算需要的总字节数

        // 步骤 1: 检查内存池中是否有足够的空间
        if (const auto bytes_left = static_cast<size_t>(end_free - start_free); bytes_left >= total_bytes)
        {
            result = start_free; // 直接从内存池中分配内存
            start_free += total_bytes; // 更新内存池起始地址
            return result;
        }
        // 步骤 2: 内存池中剩余空间足够一个块，但不够全部分配
        else if (bytes_left >= size)
        {
            nobjs = bytes_left / size; // 根据剩余空间确定可以分配多少个块
            total_bytes = nobjs * size; // 重新计算实际分配的总字节数
            result = start_free; // 从内存池中分配内存
            start_free += total_bytes; // 更新内存池起始地址
            return result;
        }
        // 步骤 3: 内存池没有足够空间，需要从系统申请新的内存块
        else
        {
            /**
             * heap_size 是已经从系统中分配的堆的大小，通过右移 4 位 (>> 4) 来计算出一个比例值，
             * 相当于 heap_size / 16。这是为了根据目前已分配的内存大小，动态地调整新分配的内存数量。
             * 这样可以让每次新申请的内存不仅与当前的需求相关，也与已经分配的内存量相关。
             */
            const size_t bytes_to_get = 2 * total_bytes + ROUND_UP(heap_size >> 4); // 计算需要申请的内存大小

            // 步骤 4: 将剩余的内存块返回到相应的自由链表中（如果有剩余空间）
            if (bytes_left > 0)
            {
                // 根据剩余空间的大小选择相应的自由链表
                obj* volatile * my_free_list = free_list + FREELIST_INDEX(bytes_left);
                // 将剩余的内存块插入到自由链表中
                reinterpret_cast<obj*>(start_free)->free_list_link = *my_free_list;
                *my_free_list = reinterpret_cast<obj*>(start_free);
            }

            // 步骤 5: 从系统中申请新的内存块
            start_free = static_cast<char*>(malloc(bytes_to_get)); // 使用 malloc 从系统分配内存
            if (nullptr == start_free) // 如果分配失败，尝试从自由链表中获取内存块
            {
                // 遍历不同大小的自由链表，尝试获取合适的内存块
                for (auto i = size; i <= MAX_BYTES; i += ALIGN)
                {
                    obj* volatile * my_free_list = free_list + FREELIST_INDEX(i); // 获取对应大小的自由链表
                    obj* p = *my_free_list; // 获取自由链表中的第一个内存块
                    if (nullptr != p)
                    {
                        // 如果自由链表中有可用的内存块，从链表中取出并更新起始和结束地址
                        *my_free_list = p->free_list_link;
                        start_free = reinterpret_cast<char*>(p);
                        end_free = start_free + i;
                        return chunk_alloc(size, nobjs); // 递归调用分配函数
                    }
                }
                end_free = nullptr; // 如果没有可用内存块，标记内存池为空
            }

            // 步骤 6: 更新内存池大小
            heap_size += bytes_to_get; // 更新总堆大小
            end_free = start_free + bytes_to_get; // 更新内存池结束地址
            return chunk_alloc(size, nobjs); // 递归调用分配函数
        }
    }


    /**
     * @brief 向自由链表补充新的内存块
     */
    template <bool threads, int ints>
    void* default_alloc_template<threads, ints>::refill(const size_t n)
    {
        int nobjs = 20; // 默认尝试分配20个块
        char* chunk = chunk_alloc(n, nobjs); // 从内存池分配nobjs个n字节的内存块
        obj* next_obj;

        // 如果只分配到了1个内存块，直接返回（此时无法补充到自由链表，只能满足本次请求）
        if (1 == nobjs)
            return chunk;

        // 找到对应大小的自由链表
        obj* volatile * my_free_list = free_list + FREELIST_INDEX(n);

        // 第一块用于返回给用户，剩下的全部挂到自由链表上
        obj* result = reinterpret_cast<obj*>(chunk); // 第一块作为返回值
        *my_free_list = next_obj = reinterpret_cast<obj*>(chunk + n); // 第二块作为自由链表头节点

        // 遍历剩余的块，将它们串联进自由链表
        for (int i = 1;; ++i)
        {
            obj* current_obj = next_obj;
            next_obj = reinterpret_cast<obj*>(reinterpret_cast<char*>(next_obj) + n); // 指向下一个块
            if (nobjs - 1 == i) // 最后一块
            {
                current_obj->free_list_link = nullptr; // 尾节点next为null
                break;
            }
            current_obj->free_list_link = next_obj; // 挂接链表
        }
        return result; // 返回第一块，供本次分配使用
    }


    /**
     * @brief 释放内存并回收到自由链表
     */
    template <bool threads, int ints>
    void default_alloc_template<threads, ints>::deallocate(void* p, const size_t n)
    {
        obj* q = static_cast<obj*>(p); // 将指针转换为 obj*，以便操作链表

        // 如果内存块大于二级分配器管理的最大块，交给一级配置器释放
        if (n > static_cast<size_t>(MAX_BYTES))
        {
            malloc_alloc::deallocate(p, n);
            return;
        }

        // 获取当前块对应的自由链表
        obj* volatile * my_free_list = free_list + FREELIST_INDEX(n);

        // 将当前块插入到自由链表头部，实现O(1)回收
        q->free_list_link = *my_free_list; // 当前块指向链表原头
        *my_free_list = q; // 当前块成为新的头节点
    }


    /**
     * @brief 重新分配内存块
     */
    template <bool threads, int ints>
    void* default_alloc_template<threads, ints>::reallocate(void* p, const size_t old_sz, const size_t new_sz)
    {
        // 情况1：新旧大小都超出二级分配器管理范围，直接调用一级配置器的reallocate
        if (old_sz > static_cast<size_t>(MAX_BYTES) && new_sz > static_cast<size_t>(MAX_BYTES))
            return malloc_alloc::reallocate(p, old_sz, new_sz);

        // 情况2：新旧大小对齐后属于同一类（在自由链表中的类别相同），直接复用原块
        if (ROUND_UP(old_sz) == ROUND_UP(new_sz))
            return p;

        // 情况3：需要分配新块，拷贝原有数据，释放旧块
        void* result = allocate(new_sz); // 分配新块
        if (result != nullptr)
        {
            // 只拷贝较小的一段数据，防止越界
            const size_t copy_sz = new_sz > old_sz ? old_sz : new_sz;
            std::memcpy(result, p, copy_sz);
            deallocate(p, old_sz); // 回收旧块
        }
        return result;
    }


    /**
     * @brief 分配内存块
     */
    template <bool threads, int ints>
    void* default_alloc_template<threads, ints>::allocate(const size_t n)
    {
        obj* result = nullptr;

        // 如果分配请求超过最大管理块，交由一级分配器处理
        if (n > static_cast<size_t>(MAX_BYTES))
            return (malloc_alloc::allocate(n));

        // 获取当前大小对应的自由链表
        obj* volatile * my_free_list = free_list + FREELIST_INDEX(n);

        result = *my_free_list; // 从自由链表头取出一个可用块

        // 如果自由链表为空，则批量分配并补充链表
        if (result == nullptr)
        {
            void* r = refill(ROUND_UP(n)); // refill 会返回一块新的内存，并补链
            return r;
        }

        // 否则，链表不为空，将链表头移到下一个
        *my_free_list = result->free_list_link;

        // 返回从链表取出的内存块
        return result;
    }

    using alloc = default_alloc_template<false, 0>; ///<默认分配器类型别名

    /**
     * @brief 通用对象分配器模板
     */
    template <typename T, typename Alloc>
    class simple_alloc
    {
    public:
        /**
         * @brief 分配 n 个 T 类型对象的内存
         */
        static T* allocate(const size_t n)
        {
            return 0 == n ? nullptr : reinterpret_cast<T*>(Alloc::allocate(n * sizeof(T)));
        }

        /**
         * @brief 分配单个 T 类型对象的内存
         */
        static T* allocate()
        {
            return static_cast<T*>(Alloc::allocate(sizeof(T)));
        }

        /**
         * @brief 释放 n 个 T 类型对象占用的内存
         */
        static void deallocate(T* p, const size_t n)
        {
            if (0 != n)
            {
                Alloc::deallocate(p, n * sizeof(T));
            }
        }

        /**
         * @brief 释放单个 T 类型对象占用的内存
         */
        static void deallocate(T* p)
        {
            Alloc::deallocate(p, sizeof(T));
        }
    };


    /**
     * @brief 针对非POD类型，在未初始化内存区域填充 n 个值
     */
    template <typename ForwardIterator, typename Size, typename T>
    ForwardIterator uninitialized_fill_n_aux_(ForwardIterator first, Size n, const T& x, false_type)
    {
        ForwardIterator cur = first;
        for (; n > 0; --n, ++cur)
            construct(&*cur, x); // 在指定位置构造对象
        return cur;
    }

    /**
     * @brief 针对POD类型，在未初始化内存区域填充 n 个值
     */
    template <typename ForwardIterator, typename Size, typename T>
    ForwardIterator uninitialized_fill_n_aux_(ForwardIterator first, Size n, const T& x, true_type)
    {
        return std::fill_n(first, n, x);
    }

    /**
     * @brief 根据类型选择正确的未初始化填充实现
     */
    template <typename ForwardIterator, typename Size, typename T, typename T1>
    ForwardIterator uninitialized_fill_n_(ForwardIterator first, Size n, const T& x, T1*)
    {
        typedef typename type_traits<T1>::is_POD_type is_POD;
        return uninitialized_fill_n_aux_(first, n, x, is_POD());
    }

    /**
     * @brief 外部接口：在未初始化内存区域填充 n 个值
     */
    template <typename ForwardIterator, typename Size, typename T>
    ForwardIterator uninitialized_fill_n(ForwardIterator first, Size n, const T& x)
    {
        // value_type(first) 用于推导目标内存区元素的类型
        return uninitialized_fill_n_(first, n, x, value_type(first));
    }


    /**
     * @brief 在未初始化内存区高效复制 char 字节序列
     */
    inline char* uninitialized_copy(const char* first, const char* last, char* result)
    {
        memmove(result, first, last - first); // 直接按字节拷贝
        return result + (last - first); // 返回拷贝后区间的末尾
    }

    /**
     * @brief 在未初始化内存区高效复制 wchar_t 字节序列
     */
    inline wchar_t* uninitialized_copy(const wchar_t* first, const wchar_t* last, wchar_t* result)
    {
        memmove(result, first, sizeof(wchar_t) * (last - first)); // 按宽字符大小拷贝
        return result + (last - first); // 返回拷贝后区间的末尾
    }

    /**
     * @brief 针对非POD类型，在未初始化内存区域逐个构造拷贝
     */
    template <typename InputIterator, typename ForwardIterator>
    ForwardIterator
    uninitialized_copy_aux_(InputIterator first, InputIterator last, ForwardIterator result, false_type)
    {
        ForwardIterator cur = result;
        for (; first != last; ++first, ++cur)
        {
            construct(&*cur, *first); // 对目标内存逐个构造新对象
        }
        return cur;
    }

    /**
     * @brief 针对POD类型，在未初始化内存区域批量拷贝
     */
    template <typename InputIterator, typename ForwardIterator>
    ForwardIterator
    uninitialized_copy_aux_(InputIterator first, InputIterator last, ForwardIterator result, true_type)
    {
        return std::copy(first, last, result);
    }

    /**
     * @brief 内部辅助：根据目标类型选择合适的拷贝方式
     */
    template <typename InputIterator, typename ForwardIterator, typename T>
    ForwardIterator uninitialized_copy_(InputIterator first, InputIterator last, ForwardIterator result, T*)
    {
        typedef typename type_traits<T>::is_POD_type is_POD;
        return uninitialized_copy_aux_(first, last, result, is_POD());
    }

    /**
     * @brief 外部接口：在未初始化内存区域拷贝区间[first, last)
     */
    template <typename InputIterator, typename ForwardIterator>
    ForwardIterator uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result)
    {
        // value_type(result) 用于类型推导，判定目标存储类型是否为POD
        return uninitialized_copy_(first, last, result, value_type(result));
    }

    /**
     * @brief 针对POD类型的未初始化区间填充
     */
    template <typename ForwardIterator, typename T>
    void
    uninitialized_fill_aux_(ForwardIterator first, ForwardIterator last, const T& x, true_type)
    {
        std::fill(first, last, x); // 直接批量赋值
    }

    /**
     * @brief 针对非POD类型的未初始化区间填充
     */
    template <typename ForwardIterator, typename T>
    void
    uninitialized_fill_aux_(ForwardIterator first, ForwardIterator last, const T& x, false_type)
    {
        ForwardIterator cur = first;
        for (; cur != last; ++cur)
            construct(&*cur, x); // 手动构造每一个对象
    }

    /**
     * @brief 内部辅助，根据类型萃取选择合适的填充方式
     */
    template <typename ForwardIterator, typename T, typename T1>
    void uninitialized_fill_(ForwardIterator first, ForwardIterator last, const T& x, T1*)
    {
        typedef typename type_traits<T1>::is_POD_type is_POD;
        uninitialized_fill_aux_(first, last, x, is_POD());
    }

    /**
     * @brief 外部接口：在未初始化区间[first, last)填充为x
     */
    template <typename ForwardIterator, typename T>
    void uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& x)
    {
        uninitialized_fill_(first, last, x, value_type(first));
    }
}


#endif
