#include "../../include/string/string.h"



namespace Tiny
{
    string::string(): m_data(new char[s_min_capacity + 1]), m_size(0), m_capacity(s_min_capacity)
    {
        m_data[0] = '\0';
    }

    string::string(const char* str)
    {
        if (str == nullptr)
        {
            throw std::invalid_argument("str is null");
        }
        m_size = std::strlen(str);
        m_capacity = std::max(m_size, s_min_capacity);
        m_data = new char[m_capacity + 1];
        std::memcpy(m_data, str, m_size);
        m_data[m_size] = '\0';
    }

    string::string(const void* str, const size_t len)
    {
        if (str == nullptr)
        {
            throw std::invalid_argument("str is null");
        }
        m_size = len;
        m_capacity = std::max(m_size, s_min_capacity);
        m_data = new char[m_capacity + 1];
        std::memcpy(m_data, str, m_size);
        m_data[m_size] = '\0';
    }

    string::string(const string& other): m_data(new char[other.m_capacity + 1]), m_size(other.m_size),
                                         m_capacity(other.m_capacity)
    {
        std::memcpy(m_data, other.m_data, m_size + 1);
    }

    string::string(string&& other) noexcept: m_data(other.m_data), m_size(other.m_size), m_capacity(other.m_capacity)
    {
        other.m_data = nullptr;
        other.m_size = 0;
        other.m_capacity = 0;
    }

    string& string::operator=(const string& other)
    {
        if (this != &other)
        {
            const auto new_data = new char[other.m_capacity + 1];
            std::memcpy(new_data, other.m_data, other.m_size + 1);
            delete[] m_data;
            m_data = new_data;
            m_size = other.m_size;
            m_capacity = other.m_capacity;
        }
        return *this;
    }

    string& string::operator=(string&& other) noexcept
    {
        if (this != &other)
        {
            delete[] m_data;
            m_data = other.m_data;
            m_size = other.m_size;
            m_capacity = other.m_capacity;
            other.m_data = nullptr;
            other.m_size = 0;
            other.m_capacity = 0;
        }
        return *this;
    }

    string& string::append(const char* str)
    {
        return append(str, std::strlen(str));
    }

    string& string::append(const char* str, const size_t len)
    {
        if (str == nullptr)
        {
            throw std::invalid_argument("str is null");
        }
        if (m_size + len > m_capacity)
        {
            reallocate((m_size + len) * 2);
        }
        std::memcpy(m_data + m_size, str, len);
        m_size += len;
        m_data[m_size] = '\0';
        return *this;
    }

    void string::shrink_to_fit()
    {
        if (m_capacity > m_size)
        {
            reallocate(m_size);
        }
    }

    const char* string::c_str() const
    {
        return m_data;
    }

    const char* string::data() const
    {
        return m_data;
    }

    size_t string::size() const
    {
        return m_size;
    }

    size_t string::capacity() const
    {
        return m_capacity;
    }

    bool string::empty() const
    {
        return m_size == 0;
    }

    char& string::operator[](const size_t index)
    {
        if (index >= m_size)
            throw std::out_of_range("index out of range");
        return m_data[index];
    }

    const char& string::operator[](const size_t index) const
    {
        if (index >= m_size)
            throw std::out_of_range("index out of range");
        return m_data[index];
    }

    bool string::operator==(const string& rhs) const
    {
        if (m_size != rhs.m_size)
            return false;
        return std::memcmp(m_data, rhs.m_data, m_size) == 0;
    }

    bool string::operator!=(const string& rhs) const
    {
        return !(*this == rhs);
    }

    void string::swap(string& other) noexcept
    {
        std::swap(m_data, other.m_data);
        std::swap(m_size, other.m_size);
        std::swap(m_capacity, other.m_capacity);
    }

    string::~string()
    {
        delete[] m_data;
    }

    void string::reallocate(size_t new_capacity)
    {
        new_capacity = std::max(new_capacity, s_min_capacity);
        const auto new_data = new char[new_capacity + 1];
        if (m_size > 0)
        {
            std::memcpy(new_data, m_data, m_size);
        }
        new_data[m_size] = '\0';
        delete[] m_data;
        m_data = new_data;
        m_capacity = new_capacity;
    }
}
