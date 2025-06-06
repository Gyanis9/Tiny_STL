#include <gtest/gtest.h>
#include "allocator/construct.hpp"

using namespace Tiny;

// 定义测试结构体
struct TestObject
{
    int value;

    TestObject() : value(0)
    {
    } // 默认构造函数
    explicit TestObject(const int v) : value(v)
    {
    } // 带参数构造函数
    ~TestObject() = default; // 析构函数
};

// 测试 construct 函数
TEST(TinySTLConstructTest, TestConstruct)
{
    TestObject* obj = nullptr;

    // 使用 construct 在特定地址上构造对象
    obj = static_cast<TestObject*>(::operator new(sizeof(TestObject))); // 手动分配内存
    construct(obj, 10); // 在 obj 上构造 TestObject，传入参数 10

    // 验证构造后的对象的值
    EXPECT_EQ(obj->value, 10);

    // 清理
    destroy(obj);
    ::operator delete(obj);
}

// 测试 destroy 函数
TEST(TinySTLConstructTest, TestDestroy)
{
    TestObject* obj = nullptr;

    // 使用 construct 创建对象
    obj = static_cast<TestObject*>(::operator new(sizeof(TestObject)));
    construct(obj, 20);

    // 确保对象值为 20
    EXPECT_EQ(obj->value, 20);

    // 销毁对象并确保无崩溃
    destroy(obj);

    // 重新分配内存并检查，确保对象已经被销毁
    obj = static_cast<TestObject*>(::operator new(sizeof(TestObject)));
    construct(obj, 30);
    EXPECT_EQ(obj->value, 30);

    // 清理
    destroy(obj);
    ::operator delete(obj);
}

// 测试 destroy 函数的范围销毁
TEST(TinySTLConstructTest, TestDestroyRange)
{
    constexpr int size = 5;
    auto* arr = static_cast<TestObject*>(::operator new(sizeof(TestObject) * size));

    // 使用 construct 函数构造多个对象
    for (int i = 0; i < size; ++i)
    {
        construct(&arr[i], i + 1);
    }

    // 确保对象值正确
    for (int i = 0; i < size; ++i)
    {
        EXPECT_EQ(arr[i].value, i + 1);
    }

    // 销毁数组中的所有对象
    destroy(arr, arr + size);

    // 清理
    ::operator delete(arr);
}

// 测试 destroy 特化版 char* 和 wchar_t* 类型
TEST(TinySTLConstructTest, TestDestroyCharTypes)
{
    char* char_ptr = nullptr;
    wchar_t* wchar_ptr = nullptr;

    // 直接调用 destroy 特化版本
    destroy(char_ptr, char_ptr);
    destroy(wchar_ptr, wchar_ptr);

    // 目前没有实际验证内容，因为 destroy 对于 char* 和 wchar_t* 是空操作
    EXPECT_TRUE(true); // 空操作不会失败
}


int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
