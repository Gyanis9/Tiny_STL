#ifndef STRING_H
#define STRING_H
#include <cstring>

namespace Tiny
{
    /**
     * @brief 一个支持小字符串优化（SSO, Small String Optimization）的简易字符串类 
     */
    class string
    {
    public:
        /**
         * @brief 默认构造函数，创建空字符串 
         */
        string();

        /**
         * @brief 由 C 风格字符串构造 string 
         */
         explicit string(const char* str);

        /**
         * @brief 由原始内存块构造 string
         */
         string(const void* data, size_t len);

        /**
         * @brief 填充构造函数
         */
        string(size_t count, char ch);

        /**
         * @brief 拷贝构造函数，深拷贝另一个 string
         */
        string(const string& other);

        /**
         * @brief 移动构造函数，接管另一个 string 的资源
         */
        string(string&& other) noexcept;

        /**
         * @brief 拷贝赋值运算符，深拷贝赋值 
         * @param other 另一个 string 对象
         */
        string& operator=(const string& other);

        /**
         * @brief 移动赋值运算符，移动赋值
         */
        string& operator=(string&& other) noexcept;

        /**
         * @brief 追加一个 C 字符串到当前字符串末尾
         */
        string& append(const char* str);

        /**
         * @brief 追加指定长度的数据到当前字符串末尾
         */
        string& append(const char* data, size_t len);

        /**
         * @brief 减小容量以适配当前字符串长度（释放多余内存） 
         */
        void shrink_to_fit();

        /**
         * @brief 获取 C 风格字符串指针，以 '\0' 结尾
         */
        [[nodiscard]] const char* c_str() const noexcept;

        /**
         * @brief 获取原始数据指针（不保证以 '\0' 结尾）
         */
        [[nodiscard]] const char* data() const noexcept;

        /**
         * @brief 获取字符串长度（不含结尾的 '\0'）
         */
        [[nodiscard]] size_t size() const noexcept;

        /**
         * @brief 获取当前容量（最大可存放多少字节，不含 '\0'）
         */
        [[nodiscard]] size_t capacity() const noexcept;

        /**
         * @brief 判断字符串是否为空
         */
        [[nodiscard]] bool empty() const noexcept;

        /**
         * @brief 获取指定下标字符的引用（可修改）
         */
        char& operator[](size_t index);

        /**
         * @brief 获取指定下标字符的常量引用
         */
        const char& operator[](size_t index) const;

        /**
         * @brief 判断两个 string 是否内容相等
         */
        bool operator==(const string& rhs) const noexcept;

        /**
         * @brief 判断两个 string 是否内容不等
         */
        bool operator!=(const string& rhs) const noexcept;

        /**
         * @brief 与另一个 string 交换内容
         */
        void swap(string& other) noexcept;

        /**
         * @brief 析构函数，释放可能分配的动态内存 
         */
        ~string();

    private:
        /**
         * @brief 确保容量足够，必要时扩容
         */
        void ensure_capacity(size_t required);

        /**
         * @brief 重新分配内存到指定容量，数据拷贝到新区域
         */
        void reallocate(size_t new_capacity);

        /**
         * @brief 判断是否使用本地（SSO）缓冲区存储
         */
        [[nodiscard]] bool using_local() const noexcept;

    private:
        static constexpr size_t SSO_CAPACITY = 15; ///< SSO缓冲区大小
        char* m_data; ///< 指向有效数据的指针（SSO模式下指向 m_local_buf）
        size_t m_size; ///< 当前字符串长度
        size_t m_capacity; ///< 当前分配的容量
        char m_local_buf[SSO_CAPACITY + 1] = {}; ///< SSO缓冲区（最后一字节为 '\0'）
    };
}

#endif
