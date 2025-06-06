#include <gtest/gtest.h>
#include "container/vector.hpp"

namespace Tiny
{
    // 测试默认构造函数
    TEST(VectorTest, DefaultConstructor)
    {
        const vector<int> v;
        EXPECT_EQ(v.size(), 0);
        EXPECT_TRUE(v.empty());
        EXPECT_EQ(v.capacity(), 0);
    }

    // 测试带大小的构造函数
    TEST(VectorTest, SizedConstructor)
    {
        vector<int> v(5);
        EXPECT_EQ(v.size(), 5);
        EXPECT_EQ(v.capacity(), 5);
        for (int i = 0; i < 5; ++i)
        {
            EXPECT_EQ(v[i], 0);
        }
    }

    // 测试带初始值的构造函数
    TEST(VectorTest, FillConstructor)
    {
        vector<std::string> v(3, "test");
        EXPECT_EQ(v.size(), 3);
        EXPECT_EQ(v.capacity(), 3);
        for (const auto& s : v)
        {
            EXPECT_EQ(s, "test");
        }
    }

    // 测试拷贝构造函数
    TEST(VectorTest, CopyConstructor)
    {
        vector<int> orig(4, 42);
        vector<int> copy(orig);

        EXPECT_EQ(orig.size(), copy.size());
        for (size_t i = 0; i < orig.size(); ++i)
        {
            EXPECT_EQ(orig[i], copy[i]);
        }
    }

    // 测试移动构造函数
    TEST(VectorTest, MoveConstructor)
    {
        vector<int> orig(100, 7);
        vector<int> moved(std::move(orig));

        EXPECT_EQ(orig.size(), 0);
        EXPECT_EQ(moved.size(), 100);
        for (int val : moved)
        {
            EXPECT_EQ(val, 7);
        }
    }

    // 测试范围构造函数
    TEST(VectorTest, RangeConstructor)
    {
        int arr[] = {1, 2, 3, 4, 5};
        vector<int> v(arr, arr + 5);

        EXPECT_EQ(v.size(), 5);
        for (int i = 0; i < 5; ++i)
        {
            EXPECT_EQ(v[i], arr[i]);
        }
    }

    // 测试元素访问
    TEST(VectorTest, ElementAccess)
    {
        vector<int> v = {10, 20, 30};
        v[1] = 200;

        EXPECT_EQ(v.front(), 10);
        EXPECT_EQ(v.back(), 30);
        EXPECT_EQ(v[0], 10);
        EXPECT_EQ(v[1], 200);
        EXPECT_EQ(v[2], 30);
    }

    // 测试 push_back 和 pop_back
    TEST(VectorTest, PushPop)
    {
        vector<int> v;
        v.push_back(1);
        v.push_back(2);
        v.push_back(3);

        EXPECT_EQ(v.size(), 3);
        EXPECT_GE(v.capacity(), 3);

        v.pop_back();
        EXPECT_EQ(v.size(), 2);
        EXPECT_EQ(v.back(), 2);
    }

    // 测试插入和扩容
    TEST(VectorTest, InsertAndGrowth)
    {
        vector<int> v;
        size_t initial_cap = 0;

        for (int i = 0; i < 100; ++i)
        {
            v.push_back(i);
            if (i == 0) initial_cap = v.capacity();
            if (v.capacity() == v.size())
            {
                EXPECT_GT(v.capacity(), initial_cap);
                initial_cap = v.capacity();
            }
        }

        EXPECT_EQ(v.size(), 100);
        for (int i = 0; i < 100; ++i)
        {
            EXPECT_EQ(v[i], i);
        }
    }

    // 测试插入函数
    TEST(VectorTest, Insert)
    {
        Tiny::vector<int> v = {1, 2, 3};
        v.insert(v.begin() + 1, 1, 4); // 在位置1插入4
        EXPECT_EQ(v.size(), 4);
        EXPECT_EQ(v[0], 1);
        EXPECT_EQ(v[1], 4);
        EXPECT_EQ(v[2], 2);
        EXPECT_EQ(v[3], 3);

        // 插入多个元素
        v.insert(v.end(), 3, 5); // 末尾插入三个5
        EXPECT_EQ(v.size(), 7);
        EXPECT_EQ(v[6], 5);
    }

    // 测试擦除函数
    TEST(VectorTest, Erase)
    {
        vector<int> v = {1, 2, 3, 4, 5};
        auto it = v.erase(v.begin() + 2); // 删除第三个元素
        EXPECT_EQ(*it, 4);
        EXPECT_EQ(v.size(), 4);
        EXPECT_EQ(v[2], 4);

        // 范围删除
        it = v.erase(v.begin(), v.begin() + 2);
        EXPECT_EQ(*it, 4);
        EXPECT_EQ(v.size(), 2);
    }

    // 测试调整大小
    TEST(VectorTest, Resize)
    {
        vector<int> v(5, 10);
        v.resize(3);
        EXPECT_EQ(v.size(), 3);

        v.resize(6, 20);
        EXPECT_EQ(v.size(), 6);
        EXPECT_EQ(v[3], 20);
        EXPECT_EQ(v[5], 20);
    }

    // 测试清空和容量管理
    TEST(VectorTest, ClearAndCapacity)
    {
        vector<int> v(100, 42);
        size_t cap = v.capacity();
        v.clear();

        EXPECT_TRUE(v.empty());
        EXPECT_EQ(v.size(), 0);
        EXPECT_EQ(v.capacity(), cap); // 容量应保持不变

        v.shrink_to_fit();
        EXPECT_EQ(v.capacity(), 0);
    }

    // 测试异常安全性
    TEST(VectorTest, ExceptionSafety)
    {
        struct TestException
        {
        };

        // 测试扩容时的异常安全
        vector<int> v;
        v.reserve(2);
        v.push_back(1);
        v.push_back(2);

        try
        {
            v.push_back(3); // 触发扩容
            ADD_FAILURE() << "Should not reach here";
        }
        catch (const TestException&)
        {
            // 验证状态一致性
            EXPECT_EQ(v.size(), 2);
            EXPECT_EQ(v.capacity(), 2);
            EXPECT_EQ(v[0], 1);
            EXPECT_EQ(v[1], 2);
        }
    }

    // // 测试自定义类型
    // TEST(VectorTest, CustomType)
    // {
    //     struct Point
    //     {
    //         int x, y;
    //
    //         Point(int x = 0, int y = 0) : x(x), y(y)
    //         {
    //         }
    //     };
    //
    //     vector<Point> points;
    //     points.emplace_back(1, 2);
    //     points.push_back(Point(3, 4));
    //
    //     EXPECT_EQ(points.size(), 2);
    //     EXPECT_EQ(points[0].x, 1);
    //     EXPECT_EQ(points[1].y, 4);
    // }

    // 测试交换功能
    TEST(VectorTest, Swap)
    {
        vector<int> a = {1, 2, 3};
        vector<int> b = {4, 5};

        a.swap(b);
        EXPECT_EQ(a.size(), 2);
        EXPECT_EQ(b.size(), 3);
        EXPECT_EQ(a[0], 4);
        EXPECT_EQ(b[2], 3);
    }
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
