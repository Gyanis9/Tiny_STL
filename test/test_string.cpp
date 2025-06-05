#include "string/string.h"
#include <gtest/gtest.h>

namespace Tiny
{
    // 测试默认构造函数
    TEST(TinyStringTest, DefaultConstructor)
    {
        const string s;
        EXPECT_EQ(s.size(), 0);
        EXPECT_GE(s.capacity(), 15);
        EXPECT_TRUE(s.empty());
        EXPECT_STREQ(s.c_str(), "");
    }

    // 测试从C字符串构造
    TEST(TinyStringTest, CStringConstructor)
    {
        // 正常字符串
        const string s1("Hello");
        EXPECT_EQ(s1.size(), 5);
        EXPECT_STREQ(s1.c_str(), "Hello");

        // 空字符串
        const string s2("");
        EXPECT_EQ(s2.size(), 0);
        EXPECT_STREQ(s2.c_str(), "");

        // nullptr
        const string s3(nullptr);
        EXPECT_EQ(s3.size(), 0);
        EXPECT_STREQ(s3.c_str(), "");
    }

    // 测试从内存块构造
    TEST(TinyStringTest, MemoryBlockConstructor)
    {
        constexpr char data[] = {'H', 'e', 'l', 'l', 'o'};

        // 正常数据
        const string s1(data, 5);
        EXPECT_EQ(s1.size(), 5);
        EXPECT_STREQ(s1.c_str(), "Hello");

        // 带空字符的数据
        constexpr char data2[] = {'H', 'i', '\0', '!'};
        string s2(data2, 4);
        EXPECT_EQ(s2.size(), 4);
        EXPECT_STREQ(s2.c_str(), "Hi");
        EXPECT_EQ(s2[2], '\0');
        EXPECT_EQ(s2[3], '!');

        // 空数据
        const string s3(nullptr, 0);
        EXPECT_EQ(s3.size(), 0);
        EXPECT_STREQ(s3.c_str(), "");

        // 零长度
        const string s4("Hello", 0);
        EXPECT_EQ(s4.size(), 0);
    }

    // 测试拷贝构造函数
    TEST(TinyStringTest, CopyConstructor)
    {
        // 小字符串
        const string s1("Hello");
        const string s2(s1);
        EXPECT_EQ(s2.size(), 5);
        EXPECT_STREQ(s2.c_str(), "Hello");
        EXPECT_EQ(s1, s2);

        // 大字符串
        const auto long_str = "This is a very long string that exceeds SSO capacity";
        const string s3(long_str);
        const string s4(s3);
        EXPECT_EQ(s3, s4);
        EXPECT_STREQ(s3.c_str(), s4.c_str());
        EXPECT_EQ(s3.capacity(), s4.capacity());

        // 空字符串
        const string s5;
        const string s6(s5);
        EXPECT_TRUE(s6.empty());
    }

    // 测试移动构造函数
    TEST(TinyStringTest, MoveConstructor)
    {
        // 小字符串
        string s1("Hello");
        const string s2(std::move(s1));
        EXPECT_EQ(s2.size(), 5);
        EXPECT_STREQ(s2.c_str(), "Hello");
        EXPECT_EQ(s1.size(), 0); // 源对象应为空
        EXPECT_STREQ(s1.c_str(), "");

        // 大字符串
        const auto long_str = "This is a very long string that exceeds SSO capacity";
        string s3(long_str);
        const auto old_data = const_cast<char*>(s3.data());
        const string s4(std::move(s3));

        EXPECT_EQ(s4.size(), strlen(long_str));
        EXPECT_STREQ(s4.c_str(), long_str);
        EXPECT_EQ(s4.data(), old_data); // 应使用相同的内存

        EXPECT_EQ(s3.size(), 0);
        EXPECT_NE(s3.data(), old_data); // 源对象应有新缓冲区
        EXPECT_STREQ(s3.c_str(), "");
    }

    // 测试拷贝赋值运算符
    TEST(TinyStringTest, CopyAssignment)
    {
        const string s1("Hello");

        // 小字符串赋值
        string s2 = s1;
        EXPECT_EQ(s1, s2);

        // 自赋值
        s2 = s2;
        EXPECT_EQ(s2.size(), 5);
        EXPECT_STREQ(s2.c_str(), "Hello");

        // 大字符串赋值
        const auto long_str = "This is a very long string that exceeds SSO capacity";
        string s3(long_str);
        string s4;
        s4 = s3;
        EXPECT_EQ(s3, s4);

        // 从SSO赋值到堆
        string s5("Short");
        s4 = s5;
        EXPECT_EQ(s4, s5);
        EXPECT_LE(s4.capacity(), 15); // 应切换回SSO

        // 从堆赋值到SSO
        string s6(long_str);
        s5 = s6;
        EXPECT_EQ(s5, s6);
        EXPECT_GT(s5.capacity(), 15);
    }

    // 测试移动赋值运算符
    TEST(TinyStringTest, MoveAssignment)
    {
        string s1("Hello");
        string s2;

        // 小字符串移动赋值
        s2 = std::move(s1);
        EXPECT_EQ(s2.size(), 5);
        EXPECT_STREQ(s2.c_str(), "Hello");
        EXPECT_EQ(s1.size(), 0);

        // 自移动赋值
        s2 = std::move(s2);
        EXPECT_EQ(s2.size(), 5);

        // 大字符串移动赋值
        const char* long_str = "This is a very long string that exceeds SSO capacity";
        string s3(long_str);
        char* old_data = const_cast<char*>(s3.data());
        string s4;
        s4 = std::move(s3);

        EXPECT_EQ(s4.size(), strlen(long_str));
        EXPECT_STREQ(s4.c_str(), long_str);
        EXPECT_EQ(s4.data(), old_data); // 应使用相同的内存

        EXPECT_EQ(s3.size(), 0);
        EXPECT_NE(s3.data(), old_data);

        // 从堆移动到SSO
        string s5("Short");
        s4 = std::move(s5);
        EXPECT_EQ(s4.size(), 5);
        EXPECT_STREQ(s4.c_str(), "Short");
        EXPECT_LE(s4.capacity(), 15);
        EXPECT_EQ(s5.size(), 0);
    }

    // 测试append功能
    TEST(TinyStringTest, Append)
    {
        string s("Hello");

        // 追加C字符串
        s.append(" World");
        EXPECT_EQ(s.size(), 11);
        EXPECT_STREQ(s.c_str(), "Hello World");

        // 追加空字符串
        s.append("");
        EXPECT_EQ(s.size(), 11);

        // 追加nullptr
        s.append(nullptr);
        EXPECT_EQ(s.size(), 11);

        // 追加内存块
        s.append("!", 1);
        EXPECT_EQ(s.size(), 12);
        EXPECT_STREQ(s.c_str(), "Hello World!");

        // 追加部分字符串
        s.append("ABC", 2);
        EXPECT_STREQ(s.c_str(), "Hello World!AB");

        // 追加到空字符串
        string s2;
        s2.append("Test");
        EXPECT_STREQ(s2.c_str(), "Test");

        // 大字符串追加触发重新分配
        string s3;
        const std::string long_str(1000, 'a');
        s3.append(long_str.c_str());
        EXPECT_EQ(s3.size(), 1000);
        EXPECT_STREQ(s3.c_str(), long_str.c_str());
    }

    // 测试shrink_to_fit
    TEST(TinyStringTest, ShrinkToFit)
    {
        // 大字符串收缩
        string s1("This is a long string that will require heap allocation");
        size_t old_cap = s1.capacity();
        s1.shrink_to_fit();
        EXPECT_EQ(s1.capacity(), old_cap);
        EXPECT_EQ(s1.size(), s1.capacity());
        EXPECT_STREQ(s1.c_str(), "This is a long string that will require heap allocation");

        // 小字符串收缩
        string s2("Short");
        old_cap = s2.capacity();
        s2.shrink_to_fit();
        EXPECT_GE(s2.capacity(), 15); // 应保持SSO容量
        EXPECT_STREQ(s2.c_str(), "Short");

        // 空字符串收缩
        string s3;
        s3.shrink_to_fit();
        EXPECT_GE(s3.capacity(), 15);
        EXPECT_TRUE(s3.empty());

        // 收缩后添加内容
        s1.append("!");
        EXPECT_GT(s1.capacity(), s1.size() - 1);
    }

    // 测试访问方法
    TEST(TinyStringTest, AccessMethods)
    {
        string s("Hello");

        // c_str 和 data
        EXPECT_STREQ(s.c_str(), "Hello");
        EXPECT_EQ(s.data(), s.c_str());

        // size 和 capacity
        EXPECT_EQ(s.size(), 5);
        EXPECT_GE(s.capacity(), 5);

        // empty
        EXPECT_FALSE(s.empty());
        string s2;
        EXPECT_TRUE(s2.empty());

        // operator[]
        EXPECT_EQ(s[0], 'H');
        EXPECT_EQ(s[4], 'o');
        s[4] = '!';
        EXPECT_STREQ(s.c_str(), "Hell!");

        // 边界检查
        EXPECT_THROW(s[5], std::out_of_range);
        EXPECT_THROW(s[100], std::out_of_range);

        // const operator[]
        const string& cs = s;
        EXPECT_EQ(cs[0], 'H');
        EXPECT_THROW(cs[5], std::out_of_range);
    }

    // 测试比较运算符
    TEST(TinyStringTest, ComparisonOperators)
    {
        string s1("Hello");
        string s2("Hello");
        string s3("World");
        string s4("Hell");

        // 相等
        EXPECT_TRUE(s1 == s2);
        EXPECT_FALSE(s1 == s3);

        // 不等
        EXPECT_TRUE(s1 != s3);
        EXPECT_FALSE(s1 != s2);

        // 不同长度
        EXPECT_FALSE(s1 == s4);
        EXPECT_TRUE(s1 != s4);

        // 空字符串
        string s5;
        string s6;
        EXPECT_TRUE(s5 == s6);
        EXPECT_FALSE(s5 != s6);
        EXPECT_FALSE(s1 == s5);

        // SSO vs 堆分配
        const std::string long_str(100, 'a');
        string s7(long_str.c_str());
        string s8(long_str.c_str());
        EXPECT_TRUE(s7 == s8);

        s8.append("b");
        EXPECT_FALSE(s7 == s8);
    }

    // 测试swap
    TEST(TinyStringTest, Swap)
    {
        // 两个SSO字符串
        string s1("Hello");
        string s2("World");
        s1.swap(s2);
        EXPECT_STREQ(s1.c_str(), "World");
        EXPECT_STREQ(s2.c_str(), "Hello");
        EXPECT_EQ(s1.size(), 5);
        EXPECT_EQ(s2.size(), 5);

        // 两个堆字符串
        const std::string long1(100, 'a');
        const std::string long2(200, 'b');
        string s3(long1.c_str());
        string s4(long2.c_str());

        s3.swap(s4);
        EXPECT_STREQ(s3.c_str(), long2.c_str());
        EXPECT_EQ(s3.size(), long2.size());
        EXPECT_STREQ(s4.c_str(), long1.c_str());
        EXPECT_EQ(s4.size(), long1.size());

        // SSO和堆字符串交换
        string s5("Short");
        string s6(long1.c_str());
        s5.swap(s6);

        // 检查s5（原堆字符串）
        EXPECT_STREQ(s5.c_str(), long1.c_str());
        EXPECT_EQ(s5.size(), long1.size());

        // 检查s6（原SSO字符串）
        EXPECT_STREQ(s6.c_str(), "Short");
        EXPECT_EQ(s6.size(), 5);

        // 自交换
        s5.swap(s5);
        EXPECT_STREQ(s5.c_str(), long1.c_str());
        EXPECT_EQ(s5.size(), long1.size());

        // 交换后修改内容
        s6.append("123");
        EXPECT_STREQ(s6.c_str(), "Short123");

        // 交换空字符串
        string s7;
        string s8("Test");
        s7.swap(s8);
        EXPECT_STREQ(s7.c_str(), "Test");
        EXPECT_TRUE(s8.empty());
    }

    // 测试SSO边界条件
    TEST(TinyStringTest, SSOBoundary)
    {
        // 刚好SSO容量
        std::string sso_max(15, 'a');
        string s1(sso_max.c_str());
        EXPECT_LE(s1.capacity(), 15);
        EXPECT_EQ(s1.size(), 15);
        EXPECT_STREQ(s1.c_str(), sso_max.c_str());

        // 超过SSO容量
        std::string heap_min(16, 'b');
        string s2(heap_min.c_str());
        EXPECT_GT(s2.capacity(), 15);
        EXPECT_EQ(s2.size(), 16);
        EXPECT_STREQ(s2.c_str(), heap_min.c_str());

        // SSO到堆转换
        string s3;
        for (int i = 0; i < 20; ++i)
        {
            s3.append("a");
        }
        EXPECT_GT(s3.capacity(), 15);
        EXPECT_EQ(s3.size(), 20);
        EXPECT_EQ(std::string(s3.c_str()), std::string(20, 'a'));
    }

    // 测试swap中的边界情况
    TEST(TinyStringTest, SwapEdgeCases)
    {
        // SSO字符串与空堆字符串交换
        string s1("Hello");
        string s2;
        s2.append("This is a long string that exceeds SSO capacity");
        s1.swap(s2);

        EXPECT_GT(s1.size(), 15);
        EXPECT_EQ(s2.size(), 5);

        // 堆字符串与空SSO字符串交换
        string s3;
        string s4("Another long string");
        s3.swap(s4);

        EXPECT_STREQ(s3.c_str(), "Another long string");
        EXPECT_TRUE(s4.empty());

        // 刚好SSO容量的字符串与堆字符串交换
        std::string sso_str(15, 'x');
        string s5(sso_str.c_str());
        string s6("Long string for testing");
        s5.swap(s6);

        EXPECT_STREQ(s5.c_str(), "Long string for testing");
        EXPECT_STREQ(s6.c_str(), sso_str.c_str());
        EXPECT_EQ(s6.size(), 15);

        // 堆字符串与刚好SSO容量的字符串交换
        s5.swap(s6);
        EXPECT_STREQ(s5.c_str(), sso_str.c_str());
        EXPECT_STREQ(s6.c_str(), "Long string for testing");
    }

    // 测试异常安全性
    TEST(TinyStringTest, ExceptionSafety)
    {
        // 拷贝赋值自赋值
        string s1("Hello");
        s1 = s1;
        EXPECT_STREQ(s1.c_str(), "Hello");

        // 移动赋值自赋值
        s1 = std::move(s1);
        EXPECT_STREQ(s1.c_str(), "Hello");

        // 下标访问越界
        EXPECT_THROW(s1[10], std::out_of_range);
        const string& cs = s1;
        EXPECT_THROW(cs[10], std::out_of_range);
    }

    // 测试内存泄漏 - 需要配合内存检测工具
    TEST(TinyStringTest, MemoryManagement)
    {
        // 多次分配释放
        for (int i = 0; i < 100; i++)
        {
            string s1("Short string");
            string s2("This is a long string that will require heap allocation");
            string s3 = s1;
            s2 = std::move(s1);
            s3.append(" additional text to force reallocation");
        }

        // 大对象赋值
        string s;
        for (int i = 0; i < 10; i++)
        {
            s = string(1000, 'a'); // 使用填充构造函数
        }
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
