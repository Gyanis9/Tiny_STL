#include <gtest/gtest.h>
#include <vector>
#include <list>
#include <string>
#include "allocator/allocator.hpp"

using namespace Tiny;

// 测试一级分配器 (malloc_alloc_template)
TEST(MallocAllocatorTest, BasicAllocation)
{
    void* p = malloc_alloc::allocate(100);
    ASSERT_NE(p, nullptr);
    malloc_alloc::deallocate(p, 100);
}

TEST(MallocAllocatorTest, Reallocation)
{
    const auto p = static_cast<int*>(malloc_alloc::allocate(10 * sizeof(int)));
    for (int i = 0; i < 10; ++i)
    {
        p[i] = i;
    }

    const auto new_p = static_cast<int*>(malloc_alloc::reallocate(p, 10 * sizeof(int), 20 * sizeof(int)));
    ASSERT_NE(new_p, nullptr);

    for (int i = 0; i < 10; ++i)
    {
        EXPECT_EQ(new_p[i], i);
    }
    malloc_alloc::deallocate(new_p, 20 * sizeof(int));
}

TEST(MallocAllocatorTest, OutOfMemoryHandler)
{
    bool handler_called = false;
    const auto old_handler = malloc_alloc::set_malloc_handler([&]()
    {
        handler_called = true;
        throw std::bad_alloc();
    });

    EXPECT_THROW({
                 // 尝试分配超大内存触发OOM
                 void* p = malloc_alloc::allocate(std::numeric_limits<size_t>::max());
                 malloc_alloc::deallocate(p, 0);
                 }, std::bad_alloc);

    EXPECT_TRUE(handler_called);
    malloc_alloc::set_malloc_handler(old_handler);
}

// 测试二级分配器 (default_alloc_template)
class DefaultAllocatorTest : public ::testing::Test
{
protected:
    using Alloc = alloc;

    void SetUp() override
    {
        // 确保分配器初始状态
        Alloc::deallocate(Alloc::allocate(1), 1);
    }
};

TEST_F(DefaultAllocatorTest, SmallAllocation)
{
    void* p = Alloc::allocate(ALIGN);
    ASSERT_NE(p, nullptr);
    Alloc::deallocate(p, ALIGN);
}

TEST_F(DefaultAllocatorTest, LargeAllocation)
{
    void* p = Alloc::allocate(MAX_BYTES + 1);
    ASSERT_NE(p, nullptr);
    Alloc::deallocate(p, MAX_BYTES + 1);
}

TEST_F(DefaultAllocatorTest, MultipleAllocations)
{
    std::vector<void*> ptrs;

    for (int i = 1; i <= MAX_BYTES; i += ALIGN)
    {
        void* p = Alloc::allocate(i);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }

    // 反向释放以测试不同大小
    for (auto it = ptrs.rbegin(); it != ptrs.rend(); ++it)
    {
        Alloc::deallocate(*it, 0); // 大小在二级分配器中不重要
    }
}

TEST_F(DefaultAllocatorTest, ReuseMemory)
{
    void* p1 = Alloc::allocate(16);
    Alloc::deallocate(p1, 16);

    void* p2 = Alloc::allocate(16);
    ASSERT_EQ(p1, p2); // 应重用相同内存块
    Alloc::deallocate(p2, 16);
}

TEST_F(DefaultAllocatorTest, Reallocation)
{
    // 小内存重分配
    auto p = static_cast<int*>(Alloc::allocate(10 * sizeof(int)));
    for (int i = 0; i < 10; ++i)
    {
        p[i] = i;
    }

    auto new_p = static_cast<int*>(Alloc::reallocate(p, 10 * sizeof(int), 20 * sizeof(int)));
    ASSERT_NE(new_p, nullptr);
    for (int i = 0; i < 10; ++i)
        EXPECT_EQ(new_p[i], i);
    Alloc::deallocate(new_p, 20 * sizeof(int));

    // 大内存重分配
    p = static_cast<int*>(Alloc::allocate(MAX_BYTES + 1));
    new_p = static_cast<int*>(Alloc::reallocate(p, MAX_BYTES + 1, MAX_BYTES * 2));
    ASSERT_NE(new_p, nullptr);
    Alloc::deallocate(new_p, MAX_BYTES * 2);
}

// 测试 simple_alloc 封装
TEST(SimpleAllocTest, TypeSpecificAllocation)
{
    using IntAlloc = simple_alloc<int, alloc>;

    int* p = IntAlloc::allocate(5);
    ASSERT_NE(p, nullptr);
    IntAlloc::deallocate(p, 5);

    int* single = IntAlloc::allocate();
    ASSERT_NE(single, nullptr);
    IntAlloc::deallocate(single);
}

TEST(SimpleAllocTest, ObjectAllocation)
{
    struct TestStruct
    {
        int a;
        double b;
    };

    using StructAlloc = simple_alloc<TestStruct, alloc>;

    TestStruct* arr = StructAlloc::allocate(10);
    ASSERT_NE(arr, nullptr);
    StructAlloc::deallocate(arr, 10);
}

// 测试未初始化内存操作
TEST(UninitializedMemoryTest, FillN)
{
    constexpr int n = 10;
    const auto arr = static_cast<int*>(alloc::allocate(n * sizeof(int)));

    uninitialized_fill_n(arr, n, 42);

    for (int i = 0; i < n; ++i)
    {
        EXPECT_EQ(arr[i], 42);
    }

    alloc::deallocate(arr, n * sizeof(int));
}

TEST(UninitializedMemoryTest, Copy)
{
    std::vector src = {1, 2, 3, 4, 5};
    const auto dest = static_cast<int*>(alloc::allocate(src.size() * sizeof(int)));

    Tiny::uninitialized_copy(src.begin(), src.end(), dest);

    for (size_t i = 0; i < src.size(); ++i)
    {
        EXPECT_EQ(dest[i], src[i]);
    }

    alloc::deallocate(dest, src.size() * sizeof(int));
}

TEST(UninitializedMemoryTest, Fill)
{
    constexpr int n = 5;
    auto* arr = static_cast<std::string*>(alloc::allocate(n * sizeof(std::string)));

    Tiny::uninitialized_fill(arr, arr + n, "test");

    for (int i = 0; i < n; ++i)
    {
        EXPECT_EQ(arr[i], "test");
    }

    // 显式析构对象
    for (int i = 0; i < n; ++i)
    {
        destroy(&arr[i]);
    }
    alloc::deallocate(arr, n * sizeof(std::string));
}

// // 综合测试：STL容器使用分配器
// TEST(IntegrationTest, VectorWithAllocator)
// {
//     using AllocVector = std::vector<int, simple_alloc<int, alloc>>;
//
//     AllocVector vec;
//     for (int i = 0; i < 100; ++i)
//     {
//         vec.push_back(i);
//     }
//
//     for (int i = 0; i < 100; ++i)
//     {
//         EXPECT_EQ(vec[i], i);
//     }
// }

// TEST(IntegrationTest, ListWithAllocator)
// {
//     using AllocList = std::list<int, simple_alloc<int, alloc>>;
//
//     AllocList lst;
//     for (int i = 0; i < 100; ++i)
//     {
//         lst.push_back(i);
//     }
//
//     int expected = 0;
//     for (const auto& val : lst)
//     {
//         EXPECT_EQ(val, expected++);
//     }
// }

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
