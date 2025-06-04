#ifndef STRING_H
#define STRING_H
#include <cstddef>
#include <cstring>
#include <algorithm>
#include <stdexcept>

namespace Tiny
{
    class string
    {
    public:
        string();

        explicit string(const char* str);

        explicit string(const void* str, size_t len);

        string(const string& other);

        string(string&& other) noexcept;

        string& operator=(const string& other);

        string& operator=(string&& other) noexcept;

        string& append(const char* str);

        string& append(const char* str, size_t len);

        void shrink_to_fit();

        [[nodiscard]] const char* c_str() const;

        [[nodiscard]] const char* data() const;

        [[nodiscard]] size_t size() const;

        [[nodiscard]] size_t capacity() const;

        [[nodiscard]] bool empty() const;

        char& operator[](size_t index);

        const char& operator[](size_t index) const;

        bool operator==(const string& rhs) const;

        bool operator!=(const string& rhs) const;

        void swap(string& other) noexcept;

        ~string();

    private:
        void reallocate(size_t new_capacity);

    private:
        static constexpr size_t s_min_capacity = 15;
        char* m_data;
        size_t m_size;
        size_t m_capacity;
    };
}


#endif
