#include "string/string.h"
#include <stdexcept>

namespace Tiny
{
    string::string(): m_data(m_local_buf),
                      m_size(0),
                      m_capacity(SSO_CAPACITY)
    {
        m_local_buf[0] = '\0';
    }

    string::string(const char* str): m_data(m_local_buf),
                                     m_size(0),
                                     m_capacity(SSO_CAPACITY)
    {
        if (str)
        {
            m_size = std::strlen(str);
            ensure_capacity(m_size);
            std::memcpy(m_data, str, m_size + 1);
        }
        else
        {
            m_local_buf[0] = '\0';
        }
    }

    string::string(const void* data, const size_t len): m_data(m_local_buf),
                                                        m_size(0),
                                                        m_capacity(SSO_CAPACITY)
    {
        if (data && len > 0)
        {
            m_size = len;
            ensure_capacity(len);
            std::memcpy(m_data, data, len);
            m_data[len] = '\0';
        }
        else
        {
            m_local_buf[0] = '\0';
        }
    }

    string::string(const size_t count, const char ch)
    {
        m_data = m_local_buf;
        m_size = 0;
        m_capacity = SSO_CAPACITY;

        if (count > 0)
        {
            ensure_capacity(count);
            std::memset(m_data, ch, count);
            m_size = count;
            m_data[m_size] = '\0';
        }
    }

    string::string(const string& other): m_data(m_local_buf),
                                         m_size(other.m_size),
                                         m_capacity(SSO_CAPACITY)
    {
        if (other.using_local())
        {
            std::memcpy(m_local_buf, other.m_local_buf, SSO_CAPACITY + 1);
        }
        else
        {
            ensure_capacity(other.m_size);
            std::memcpy(m_data, other.m_data, other.m_size + 1);
        }
    }

    string::string(string&& other) noexcept: m_data(m_local_buf),
                                             m_size(other.m_size),
                                             m_capacity(other.m_capacity)
    {
        if (other.using_local())
        {
            std::memcpy(m_local_buf, other.m_local_buf, SSO_CAPACITY + 1);
        }
        else
        {
            m_data = other.m_data;
        }
        // 将原对象重置为SSO状态
        other.m_data = other.m_local_buf;
        other.m_size = 0;
        other.m_capacity = SSO_CAPACITY;
        other.m_local_buf[0] = '\0';
    }

    string& string::operator=(const string& other)
    {
        if (this != &other)
        {
            string temp(other);
            swap(temp);
        }
        return *this;
    }

    string& string::operator=(string&& other) noexcept
    {
        if (this != &other)
        {
            // 释放当前资源
            if (!using_local())
            {
                delete[] m_data;
            }


            if (other.using_local())
            {
                std::memcpy(m_local_buf, other.m_local_buf, SSO_CAPACITY + 1);
                m_data = m_local_buf;
            }
            else
            {
                m_data = other.m_data;
            }
            // 接管资源
            m_size = other.m_size;
            m_capacity = other.m_capacity;
            other.m_data = other.m_local_buf;
            other.m_size = 0;
            other.m_capacity = SSO_CAPACITY;
            other.m_local_buf[0] = '\0';
        }
        return *this;
    }

    string& string::append(const char* str)
    {
        if (str)
        {
            const size_t len = std::strlen(str);
            append(str, len);
        }
        return *this;
    }

    string& string::append(const char* data, const size_t len)
    {
        if (data && len > 0)
        {
            const size_t new_size = m_size + len;
            ensure_capacity(new_size);
            std::memcpy(m_data + m_size, data, len);
            m_size = new_size;
            m_data[m_size] = '\0';
        }
        return *this;
    }

    void string::shrink_to_fit()
    {
        if (m_size < m_capacity && !using_local())
        {
            if (m_size <= SSO_CAPACITY)
            {
                // 切换回SSO
                char* new_data = m_local_buf;
                std::memcpy(new_data, m_data, m_size + 1);
                delete[] m_data;
                m_data = new_data;
                m_capacity = SSO_CAPACITY;
            }
            else
            {
                // 重新分配精确大小的堆内存
                reallocate(m_size);
            }
        }
    }

    const char* string::c_str() const noexcept
    {
        return m_data;
    }

    const char* string::data() const noexcept
    {
        return m_data;
    }

    size_t string::size() const noexcept
    {
        return m_size;
    }

    size_t string::capacity() const noexcept
    {
        return m_capacity;
    }

    bool string::empty() const noexcept
    {
        return m_size == 0;
    }

    char& string::operator[](const size_t index)
    {
        if (index >= m_size)
        {
            throw std::out_of_range("Index out of range");
        }
        return m_data[index];
    }

    const char& string::operator[](const size_t index) const
    {
        if (index >= m_size)
        {
            throw std::out_of_range("Index out of range");
        }
        return m_data[index];
    }

    bool string::operator==(const string& rhs) const noexcept
    {
        if (m_size != rhs.m_size) return false;
        return std::memcmp(m_data, rhs.m_data, m_size) == 0;
    }

    bool string::operator!=(const string& rhs) const noexcept
    {
        return !(*this == rhs);
    }


    void string::swap(string& other) noexcept
    {
        // 处理SSO和堆的混合情况
        if (using_local() && other.using_local())
        {
            // 两个都是SSO - 交换缓冲区内容
            char temp[SSO_CAPACITY + 1];
            std::memcpy(temp, m_local_buf, SSO_CAPACITY + 1);
            std::memcpy(m_local_buf, other.m_local_buf, SSO_CAPACITY + 1);
            std::memcpy(other.m_local_buf, temp, SSO_CAPACITY + 1);
            std::swap(m_size, other.m_size);
        }
        else if (!using_local() && !other.using_local())
        {
            // 两个都是堆 - 交换指针
            std::swap(m_data, other.m_data);
            std::swap(m_size, other.m_size);
            std::swap(m_capacity, other.m_capacity);
        }
        else
        {
            // 混合情况：一个SSO一个堆
            // 使用临时缓冲区保存SSO内容
            char temp_buf[SSO_CAPACITY + 1];
            size_t temp_size;

            if (using_local())
            {
                // 当前对象是SSO，other是堆
                // 保存当前SSO内容
                std::memcpy(temp_buf, m_local_buf, SSO_CAPACITY + 1);
                temp_size = m_size;

                // 当前对象接管other的堆内存
                m_data = other.m_data;
                m_size = other.m_size;
                m_capacity = other.m_capacity;

                // other接管当前对象的SSO内容
                other.m_data = other.m_local_buf;
                std::memcpy(other.m_local_buf, temp_buf, SSO_CAPACITY + 1);
                other.m_size = temp_size;
                other.m_capacity = SSO_CAPACITY;
            }
            else
            {
                // 当前对象是堆，other是SSO
                // 保存other的SSO内容
                std::memcpy(temp_buf, other.m_local_buf, SSO_CAPACITY + 1);
                temp_size = other.m_size;

                // other接管当前对象的堆内存
                other.m_data = m_data;
                other.m_size = m_size;
                other.m_capacity = m_capacity;

                // 当前对象接管other的SSO内容
                m_data = m_local_buf;
                std::memcpy(m_local_buf, temp_buf, SSO_CAPACITY + 1);
                m_size = temp_size;
                m_capacity = SSO_CAPACITY;
            }
        }
    }


    string::~string()
    {
        if (!using_local())
        {
            delete[] m_data;
        }
    }

    void string::ensure_capacity(const size_t required)
    {
        if (required > m_capacity)
        {
            const size_t new_capacity = std::max(m_capacity * 2, required);
            reallocate(new_capacity);
        }
    }

    void string::reallocate(const size_t new_capacity)
    {
        const auto new_data = new char[new_capacity + 1];
        std::memcpy(new_data, m_data, m_size + 1);

        if (!using_local())
        {
            delete[] m_data;
        }

        m_data = new_data;
        m_capacity = new_capacity;
    }

    bool string::using_local() const noexcept
    {
        return m_data == m_local_buf;
    }
}
