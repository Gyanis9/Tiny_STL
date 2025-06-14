//
// Created by guo on 24-11-21.
//

#ifndef TINY_STL_VECTOR_HPP
#define TINY_STL_VECTOR_HPP

#include "../allocator/allocator.hpp"

namespace Tiny {

    template<typename T, typename Alloc=alloc>
    class vector {
    public:
        typedef T value_type;
        typedef value_type *pointer;
        typedef value_type *iterator;
        typedef value_type &reference;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;

    protected:
        typedef simple_alloc<value_type, Alloc> data_allocator;
        iterator start;
        iterator finish;
        iterator end_of_storage;


        void insert_aux(iterator position, const T &x);

        void deallocate() {
            if (start)
                data_allocator::deallocate(start, end_of_storage - start);
        }

        /**配置空间并填满内容*/
        iterator alloc_and_fill(size_type n, const T &value) {
            iterator result = data_allocator::allocate(n);
            uninitialized_fill_n(result, n, value);
            return result;
        }

        void fill_initialize(size_type n, const T &value) {
            start = alloc_and_fill(n, value);
            finish = start + n;
            end_of_storage = finish;
        }

    public:
        iterator begin() { return start; }

        iterator begin() const { return start; }

        iterator end() { return finish; }

        iterator end() const { return finish; }

        size_type size() const { return static_cast<size_type >(end() - begin()); }

        size_type capacity() const { return static_cast<size_type >(end_of_storage - begin()); }

        bool empty() const { return begin() == end(); }

        reference operator[](size_type n) { return *(begin() + n); }

        vector() : start(nullptr), finish(nullptr), end_of_storage(nullptr) {}

        vector(size_type n, const T &value) { fill_initialize(n, value); }

        vector(int n, const T &value) { fill_initialize(n, value); }

        vector(long n, const T &value) { fill_initialize(n, value); }

        explicit vector(size_type n) { fill_initialize(n, T()); }

        ~vector() {
            destroy(start, finish);
            deallocate();
        }

        reference front() { return *begin(); }

        reference back() { return *(end() - 1); }

        void push_back(const T &x) {
            if (finish != end_of_storage) {
                construct(finish, x);
                ++finish;
            } else {
                insert_aux(end(), x);
            }
        }

        void pop_back() {
            --finish;
            destroy(finish);
        }

        iterator erase(iterator position) {
            if (position + 1 != end())
                std::copy(position + 1, finish, position);
            --finish;
            destroy(finish);
            return position;
        }

        iterator erase(iterator first, iterator last) {
            iterator i = std::copy(last, finish, first);
            destroy(i, finish);
            finish -= last - first;
            return first;
        }

        void resize(size_type new_size, const T &x) {
            if (new_size < size())
                erase(begin() + new_size, end());
            else
                insert(end(), new_size - size(), x);
        }

        void resize(size_type new_size) { resize(new_size, T()); }

        void clear() { erase(begin(), end()); }

        void insert(iterator position, size_type n, const T &x);


    };

    template<typename T, typename Alloc>
    void vector<T, Alloc>::insert(vector::iterator position, vector::size_type n, const T &x) {
        if (n != 0) {
            if (static_cast<size_type>(end_of_storage - finish) >= n) {/**备用空间大于等于新增元素个数*/
                T x_copy = x;
                const size_type elems_after = finish - position;
                iterator old_finish = finish;
                if (elems_after > n) {/**插入点之后的现有元素个数大于新增元素个数*/
                    uninitialized_copy(finish - n, finish, finish);
                    finish += n;
                    std::copy_backward(position, old_finish - n, old_finish);
                    std::fill(position, position + n, x_copy);
                } else {/**插入点之后的现有元素个数小于等于新增元素个数*/
                    uninitialized_fill_n(finish, n - elems_after, x_copy);
                    finish += n - elems_after;
                    uninitialized_copy(position, old_finish, finish);
                    finish += elems_after;
                    std::fill(position, old_finish, x_copy);
                }
            } else {/**备用空间小于新增元素个数*/
                const size_type old_size = size();
                const size_type len = old_size + std::max(old_size, n);
                iterator new_start = data_allocator::allocate(len);
                iterator new_finish = new_start;
                try {
                    new_finish = uninitialized_copy(start, position, new_start);
                    new_finish = uninitialized_fill_n(new_finish, n, x);
                    new_finish = uninitialized_copy(position, finish, new_finish);
                }
                catch (...) {
                    destroy(new_start, new_finish);
                    data_allocator::deallocate(new_start, len);
                    throw;
                }
                destroy(start, finish);
                deallocate();
                start = new_start;
                finish = new_finish;
                end_of_storage = new_start + len;
            }
        }
    }

    template<typename T, typename Alloc>
    void vector<T, Alloc>::insert_aux(vector::iterator position, const T &x) {
        if (end() != end_of_storage) {
            construct(finish, *(finish - 1));
            ++finish;
            T x_copy = x;
            std::copy_backward(position, finish - 2, finish - 1);
            *position = x_copy;
        } else {
            const size_type old_size = size();
            const size_type len = old_size != 0 ? 2 * old_size : 1;
            iterator new_start = data_allocator::allocate(len);
            iterator new_finish = new_start;
            try {
                new_finish = uninitialized_copy(start, position, new_start);
                construct(new_finish, x);
                ++new_finish;
                new_finish = uninitialized_copy(position, finish, new_finish);
            }
            catch (...) {
                destroy(new_start, new_finish);
                data_allocator::deallocate(new_start, len);
                throw;
            }
            destroy(begin(), end());
            deallocate();
            start = new_start;
            finish = new_finish;
            end_of_storage = new_start + len;
        }
    }
}


#endif //TINY_STL_VECTOR_HPP
