#include <gtest/gtest.h>
#include "utility/RingBuffer.h"
#include <thread>
#include <vector>
#include <atomic>
#include <type_traits>

using namespace Tiny;
// 测试基本功能
TEST(RingBufferTest, BasicOperations)
{
    RingBuffer<int, 8> buffer;

    // 空缓冲区检查
    EXPECT_TRUE(buffer.Empty());
    EXPECT_FALSE(buffer.Full());
    EXPECT_EQ(buffer.Size(), 0);

    // 添加元素
    EXPECT_TRUE(buffer.Push(1));
    EXPECT_FALSE(buffer.Empty());
    EXPECT_EQ(buffer.Size(), 1);

    // 取出元素
    int value = 0;
    EXPECT_TRUE(buffer.Pop(value));
    EXPECT_EQ(value, 1);
    EXPECT_TRUE(buffer.Empty());
}

// 测试循环填充
TEST(RingBufferTest, CircularBehavior)
{
    RingBuffer<int, 4> buffer; // 实际容量3

    // 填充到满
    for (int i = 1; i <= 3; ++i)
    {
        EXPECT_TRUE(buffer.Push(i));
    }
    EXPECT_TRUE(buffer.Full());
    EXPECT_EQ(buffer.Size(), 3);

    // 满时添加失败
    EXPECT_FALSE(buffer.Push(4));

    // 取出所有元素
    for (int i = 1; i <= 3; ++i)
    {
        int value = 0;
        EXPECT_TRUE(buffer.Pop(value));
        EXPECT_EQ(value, i);
    }
    EXPECT_TRUE(buffer.Empty());

    // 再次填充
    for (int i = 10; i <= 12; ++i)
    {
        EXPECT_TRUE(buffer.Push(i));
    }
    EXPECT_TRUE(buffer.Full());
}

// 测试大小计算
TEST(RingBufferTest, SizeCalculation)
{
    RingBuffer<int, 16> buffer;

    for (int i = 0; i < 10; ++i)
    {
        buffer.Push(i);
        EXPECT_EQ(buffer.Size(), i + 1);
    }

    for (int i = 0; i < 5; ++i)
    {
        int value;
        buffer.Pop(value);
        EXPECT_EQ(buffer.Size(), 9 - i);
    }
}

// 测试移动语义
TEST(RingBufferTest, MoveSemantics)
{
    struct Movable
    {
        int value;
        bool moved = false;

        explicit Movable(const int v) : value(v)
        {
        }

        Movable(Movable&& other) noexcept : value(other.value)
        {
            other.moved = true;
        }

        Movable& operator=(Movable&& other) noexcept
        {
            value = other.value;
            other.moved = true;
            return *this;
        }
    };

    RingBuffer<Movable, 4> buffer;

    Movable m1(42);
    buffer.Push(std::move(m1));
    EXPECT_TRUE(m1.moved); // 确认移动发生

    Movable m2(0);
    buffer.Pop(m2);
    EXPECT_EQ(m2.value, 42);
}

// 测试异常安全
TEST(RingBufferTest, ExceptionSafety)
{
    struct ThrowingType
    {
        bool throwOnCopy;
        ThrowingType() = default;

        ThrowingType(const ThrowingType& other) : throwOnCopy(other.throwOnCopy)
        {
            if (throwOnCopy) throw std::runtime_error("Copy failed");
        }
    };

    RingBuffer<ThrowingType, 4> buffer;

    ThrowingType safe{};
    safe.throwOnCopy = false;
    EXPECT_TRUE(buffer.Push(safe));

    ThrowingType unsafe{};
    unsafe.throwOnCopy = true;
    EXPECT_FALSE(buffer.Push(unsafe)); // 构造失败

    // 缓冲区状态应保持不变
    EXPECT_EQ(buffer.Size(), 1);
    EXPECT_FALSE(buffer.Full());
}

// 测试并发安全性 - 单生产者单消费者
TEST(RingBufferTest, ConcurrentSPSC)
{
    constexpr size_t CAPACITY = 1024;
    RingBuffer<int, CAPACITY> buffer;
    constexpr int TOTAL_ITEMS = 1000000;
    std::atomic<bool> producer_done(false);
    std::atomic<int> consumed_count(0);

    // 生产者线程
    auto producer = [&]()
    {
        for (int i = 0; i < TOTAL_ITEMS;)
        {
            if (buffer.Push(i))
            {
                i++;
            }
        }
        producer_done = true;
    };

    // 消费者线程
    auto consumer = [&]()
    {
        int last_value = -1;
        while (!producer_done || !buffer.Empty())
        {
            if (int value; buffer.Pop(value))
            {
                EXPECT_EQ(value, last_value + 1);
                last_value = value;
                ++consumed_count;
            }
        }
    };

    std::thread prod_thread(producer);
    std::thread cons_thread(consumer);

    prod_thread.join();
    cons_thread.join();

    EXPECT_EQ(consumed_count, TOTAL_ITEMS);
    EXPECT_TRUE(buffer.Empty());
}

// 测试不同类型和容量
TEST(RingBufferTest, VariousTypes)
{
    // 大对象测试
    struct LargeObject
    {
        char data[1024];
        int id;
    };

    RingBuffer<LargeObject, 8> largeBuffer;
    for (int i = 0; i < 5; ++i)
    {
        LargeObject obj{};
        obj.id = i;
        EXPECT_TRUE(largeBuffer.Push(obj));
    }

    // 字符串测试
    RingBuffer<std::string, 8> strBuffer;
    EXPECT_TRUE(strBuffer.Push("Hello"));
    EXPECT_TRUE(strBuffer.Push("World"));

    std::string s1, s2;
    strBuffer.Pop(s1);
    strBuffer.Pop(s2);
    EXPECT_EQ(s1, "Hello");
    EXPECT_EQ(s2, "World");
}

// 测试完美转发
TEST(RingBufferTest, Emplace)
{
    struct Point
    {
        int x, y;

        Point(const int x, const int y) : x(x), y(y)
        {
        }

        ~Point() = default;
    };

    RingBuffer<Point, 4> buffer;

    // 直接构造对象
    buffer.Emplace(3, 4);

    Point p(0, 0);
    buffer.Pop(p);
    EXPECT_EQ(p.x, 3);
    EXPECT_EQ(p.y, 4);
}

// 测试边界容量
TEST(RingBufferTest, EdgeCapacity)
{
    // 容量1（实际为0，但static_assert会阻止）
    // 改为测试最小有效容量2（实际容量1）
    RingBuffer<int, 2> buffer; // 实际容量1

    EXPECT_TRUE(buffer.Push(42));
    EXPECT_TRUE(buffer.Full());
    EXPECT_FALSE(buffer.Push(43));

    int value;
    EXPECT_TRUE(buffer.Pop(value));
    EXPECT_EQ(value, 42);
    EXPECT_TRUE(buffer.Empty());
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
