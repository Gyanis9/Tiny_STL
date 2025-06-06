#ifndef RANGBUFFER_H
#define RANGBUFFER_H

#include <atomic>
#include <type_traits>

namespace Tiny
{
    /**
     * @brief 固定容量的无锁环形缓冲区（环形队列），支持并发环境下的单生产者单消费者模式
     */
    template <typename T, size_t Capacity>
    class RingBuffer
    {
        // 静态断言：容量必须为2的幂，以支持位运算取模
        static_assert(Capacity && !(Capacity & (Capacity - 1)),
                      "Capacity must be a power of 2");

    public:
        /**
         * @brief 构造函数，初始化头尾指针
         */
        RingBuffer() = default;

        /**
         * @brief 析构函数，依次析构所有尚未出队的元素
         */
        ~RingBuffer()
        {
            size_t rear = m_head.load(std::memory_order_relaxed);
            const size_t front = m_tail.load(std::memory_order_relaxed);
            while (rear != front)
            {
                reinterpret_cast<T*>(&m_buffer[rear])->~T();
                rear = (rear + 1) & (Capacity - 1);
            }
        }

        /**
         * @brief 拷贝入队，将元素拷贝到缓冲区尾部
         */
        bool Push(const T& item)
        {
            return Emplace(item);
        }

        /**
         * @brief 移动入队，将元素移动到缓冲区尾部
         */
        bool Push(T&& item)
        {
            return Emplace(std::move(item));
        }

        /**
         * @brief 完美转发入队，原地构造元素到缓冲区尾部
         */
        template <typename... Args>
        bool Emplace(Args&&... args)
        {
            static_assert(std::is_constructible_v<T, Args...>);
            const size_t tail = m_tail.load(std::memory_order_relaxed);
            const size_t next = (tail + 1) & (Capacity - 1);

            if (next == m_head.load(std::memory_order_acquire))
                return false;

            try
            {
                // 完美转发任意数量参数
                new(&m_buffer[tail]) T(std::forward<Args>(args)...);
            }
            catch (...)
            {
                return false;
            }

            m_tail.store(next, std::memory_order_release);
            return true;
        }


        /**
         * @brief 出队，将队首元素移动到item中，并析构原始对象
         */
        bool Pop(T& item) noexcept(std::is_nothrow_move_assignable_v<T>)
        {
            const size_t head = m_head.load(std::memory_order_relaxed);
            // 队空
            if (head == m_tail.load(std::memory_order_acquire))
                return false;

            T* elem = reinterpret_cast<T*>(&m_buffer[head]);
            item = std::move(*elem); // 移动元素
            elem->~T(); // 显式析构

            // 推进头指针
            m_head.store((head + 1) & (Capacity - 1), std::memory_order_release);
            return true;
        }

        /**
         * @brief 获取当前队列中元素数量
         */
        [[nodiscard]] size_t Size() const noexcept
        {
            const size_t head = m_head.load(std::memory_order_acquire);
            const size_t tail = m_tail.load(std::memory_order_acquire);
            return (tail - head) & (Capacity - 1);
        }

        /**
         * @brief 判断队列是否为空
         */
        [[nodiscard]] bool Empty() const noexcept
        {
            return m_head.load(std::memory_order_acquire) ==
                m_tail.load(std::memory_order_acquire);
        }

        /**
         * @brief 判断队列是否已满
         */
        [[nodiscard]] bool Full() const noexcept
        {
            const size_t head = m_head.load(std::memory_order_acquire);
            const size_t next_tail = (m_tail.load(std::memory_order_relaxed) + 1) & (Capacity - 1);
            return next_tail == head;
        }

    private:
        alignas(64) std::atomic<size_t> m_head{0}; ///< 队首索引（头指针），用于出队
        alignas(64) std::atomic<size_t> m_tail{0}; ///< 队尾索引（尾指针），用于入队
        alignas(alignof(T)) std::aligned_storage_t<sizeof(T), alignof(T)> m_buffer[Capacity]; ///< 原始存储空间，避免T必须有默认构造
    };
}

#endif
