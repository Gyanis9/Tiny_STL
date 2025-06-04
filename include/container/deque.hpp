//
// Created by guo on 24-11-27.
//

#ifndef TINY_STL_DEQUE_HPP
#define TINY_STL_DEQUE_HPP

#include "../allocator/allocator.hpp"

namespace Tiny {
    inline size_t _deque_buf_size(size_t n, size_t sz) {
        return n != 0 ? n : (sz < 512 ? static_cast<size_t>(512 / sz) : static_cast<size_t>(1));
    }

    template<typename T, typename Ref, typename Ptr, size_t BufSize>
    struct _deque_iterator {
        typedef _deque_iterator<T, T &, T *, BufSize> iterator;
        typedef _deque_iterator<T, const T &, const T *, BufSize> const_iterator;

        static size_t buffer_size() { return _deque_buf_size(BufSize, sizeof(T)); }

        typedef random_access_iterator_tag iterator_category;
        typedef size_t size_type;
        typedef T value_type;
        typedef Ptr pointer;
        typedef Ref reference;
        typedef ptrdiff_t difference_type;

        typedef value_type **map_pointer;

        typedef _deque_iterator self;

        /**分别指向一小段内容的当前节点和首尾位置以及当前迭代器指向的map节点*/
        T *cur;
        T *first;
        T *last;
        map_pointer node;

        /**设置当前迭代器指向的mao节点*/
        void set_node(map_pointer new_node) {
            node = new_node;
            first = *node;
            last = first + static_cast<difference_type >(buffer_size());
        }

        reference operator*() const { return *cur; }

        pointer operator->() const { return &(operator*()); }

        /**两个迭代器相减*/
        difference_type operator-(const self &x) const {
            return static_cast<difference_type >(buffer_size()) * (node - x.node - 1) + (cur - first) +
                   (x.last - x.cur);
        }

        self &operator++() {
            ++cur;
            if (cur == last) {
                set_node(node + 1);
                cur = first;
            }
            return *this;
        }

        self operator++(int) {
            self temp = *this;
            ++*this;
            return temp;
        }

        self &operator--() {
            if (cur == first) {
                set_node(node - 1);
                cur = last;
            }
            --cur;
            return *this;
        }

        self operator--(int) {
            self temp = *this;
            --*this;
            return temp;
        }

        self operator+=(difference_type n) {
            difference_type offset = n + (cur - first);
            if (offset >= 0 && offset < static_cast<difference_type>(buffer_size()))
                cur += n;
            else {
                difference_type node_offset = offset > 0 ? offset / static_cast<difference_type >(buffer_size()) :
                                              -static_cast<difference_type >((-offset - 1) / buffer_size()) - 1;
                set_node(node + node_offset);
                cur = first + (offset - node_offset * static_cast<difference_type>(buffer_size()));
            }
            return *this;
        }

        self operator+(difference_type n) const {
            self temp = *this;
            temp += n;
            return temp;
        }

        self &operator-=(difference_type n) { return *this += -n; }

        self operator-(difference_type n) const {
            self temp = *this;
            return temp -= n;
        }

        reference operator[](difference_type n) { return *(*this + n); }

        bool operator==(const self &x) const { return cur == x.cur; }

        /**note:不可使用return *this != x; ,*this != x 实际上会再次调用 operator!= 函数，而没有任何终止条件。这就造成了递归调用*/
        bool operator!=(const self &x) const { return !(*this == x); }

        bool operator<(const self &x) const { return (node == x.node) ? (cur < x.cur) : (node < x.node); }
    };

    template<typename T, typename Alloc=alloc, size_t BufSize = 0>
    class deque {
    public:
        /**基本数据类型*/
        typedef T value_type;
        typedef value_type *pointer;
        typedef value_type &reference;
        typedef const value_type &const_reference;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;

    public:
        /**迭代器*/
        typedef _deque_iterator<T, reference, pointer, BufSize> iterator;
        typedef _deque_iterator<T, reference, const pointer, BufSize> const_iterator;

    protected:
        /**map指针以及内存池分配器*/
        typedef pointer *map_pointer;
        typedef simple_alloc<value_type, Alloc> data_allocator;
        typedef simple_alloc<pointer, Alloc> map_allocator;

        pointer allocate_node() {
            return data_allocator::allocate(_deque_iterator<T, T &, T *, BufSize>::buffer_size());
        }

        void deallocate_node(pointer p) {
            data_allocator::deallocate(p, _deque_iterator<T, T &, T *, BufSize>::buffer_size());
        }


        void create_map_and_node(size_type num_elements);

        size_type initial_map_size() { return default_Node_size; }

        void fill_initialize(size_type n, const value_type &value);

        void push_back_aux(const value_type &t);

        void push_front_aux(const value_type &t);

        void reallocate_map(size_type nodes_to_add, bool add_at_front);

        void reserve_map_at_back(size_type nodes_to_add = 1) {
            if (nodes_to_add + 1 > map_size - (finish.node - map))
                reallocate_map(nodes_to_add, false);
        }

        void reserve_map_at_front(size_type nodes_to_add = 1) {
            if (nodes_to_add > start.node - map)
                reallocate_map(nodes_to_add, true);
        }

        void pop_back_aux();

        void pop_font_aux();

        iterator insert_aux(iterator pos, const value_type &x);

    protected:
        iterator start;
        iterator finish;
        map_pointer map;
        size_type map_size;
        size_type default_Node_size = 8;

    public:
        deque(int n, const value_type &x) : start(), finish(), map(nullptr), map_size(0) {
            fill_initialize(n, x);
        }

        iterator begin() { return start; }

        iterator begin() const { return start; }

        iterator end() { return finish; }

        iterator end() const { return finish; }

        reference operator[](size_type n) { return start[static_cast<difference_type>(n)]; }

        reference front() { return *start; }

        reference back() {
            iterator temp = finish;
            --temp;
            return *temp;
        }

        size_type size() const { return finish - start; }

        size_type max_size() const { return static_cast<size_type>(-1); }

        bool empty() const { return finish == start; }

        void push_back(const value_type &t);

        void push_front(const value_type &t);

        void pop_back();

        void pop_front();

        void clear();

        void erase(iterator pos);

        void erase(iterator first, iterator last);

        iterator insert(iterator position, const value_type &x);

    };

    template<typename T, typename Alloc, size_t BufSize>
    typename deque<T, Alloc, BufSize>::iterator
    deque<T, Alloc, BufSize>::insert_aux(deque::iterator pos, const value_type &x) {
        difference_type index = pos - start;
        value_type x_copy = x;
        if (index < (size() / 2)) {
            push_front(front());
            iterator front1 = start;
            ++front1;
            iterator front2 = front1;
            ++front2;
            pos = start + index;
            iterator pos1 = pos;
            ++pos1;
            std::copy(front2, pos1, front1);
        } else {
            push_back(back());
            iterator back1 = finish;
            --back1;
            iterator back2 = back1;
            --back2;
            pos = start + index;
            std::copy_backward(pos, back2, back1);
        }
        *pos = x_copy;
        return pos;
    }


    template<typename T, typename Alloc, size_t BufSize>
    typename deque<T, Alloc, BufSize>::iterator
    deque<T, Alloc, BufSize>::insert(deque::iterator position, const value_type &x) {
        if (position.cur == start.cur) {
            push_front(x);
            return start;
        } else if (position.cur == finish.cur) {
            push_back(x);
            iterator temp = finish;
            --temp;
            return temp;
        } else {
            return insert_aux(position, x);
        }
    }


    template<typename T, typename Alloc, size_t BufSize>
    void deque<T, Alloc, BufSize>::erase(deque::iterator first, deque::iterator last) {
        if (first == start && last == finish) {
            clear();
            return;
        } else {
            difference_type n = last - first;
            difference_type elems_before = first - start;
            if (elems_before < (size() - n) / 2) {
                std::copy_backward(start, first, last);
                iterator new_start = start + n;
                destroy(start, new_start);

                for (map_pointer cur = start.node; cur < new_start.node; ++cur)
                    data_allocator::deallocate(*cur, _deque_iterator<T, T &, T *, BufSize>::buffer_size());
                start = new_start;
            } else {
                std::copy(last, finish, first);
                iterator new_finish = finish - n;
                destroy(new_finish, finish);
                for (map_pointer cur = new_finish.node + 1; cur <= finish.node; ++cur)
                    data_allocator::deallocate(*cur, _deque_iterator<T, T &, T *, BufSize>::buffer_size());
                finish = new_finish;
            }
            return start + elems_before;
        }
    }

    template<typename T, typename Alloc, size_t BufSize>
    void deque<T, Alloc, BufSize>::erase(deque::iterator pos) {
        iterator next = pos;
        ++next;
        difference_type index = pos - start;
        if (index < (size() >> 1)) {
            std::copy_backward(start, pos, next);
            pop_front();
        } else {
            std::copy(next, finish, pos);
            pop_back();
        }
        return start + index;
    }

    template<typename T, typename Alloc, size_t BufSize>
    void deque<T, Alloc, BufSize>::clear() {
        for (map_pointer node = start.node + 1; node < finish.node; ++node) {
            destroy(*node, *node + _deque_iterator<T, T &, T *, BufSize>::buffer_size());
            data_allocator::deallocate(*node, _deque_iterator<T, T &, T *, BufSize>::buffer_size());
        }
        if (start.node != finish.node) {
            destroy(start.cur, start.last);
            destroy(finish.first, finish.cur);
            data_allocator::deallocate(finish.first, _deque_iterator<T, T &, T *, BufSize>::buffer_size());
        } else
            destroy(start.cur, finish.cur);
        finish = start;
    }

    template<typename T, typename Alloc, size_t BufSize>
    void deque<T, Alloc, BufSize>::pop_font_aux() {
        destroy(start.cur);
        deallocate_node(start.first);
        start.set_node(start.node + 1);
        start.cur = start.first;
    }

    template<typename T, typename Alloc, size_t BufSize>
    void deque<T, Alloc, BufSize>::pop_front() {
        if (start.cur != start.last - 1) {
            destroy(start.cur);
            ++start.cur;
        } else
            pop_font_aux();
    }

    template<typename T, typename Alloc, size_t BufSize>
    void deque<T, Alloc, BufSize>::pop_back_aux() {
        deallocate_node(finish.first);
        finish.set_node(finish.node - 1);
        finish.cur = finish.last - 1;
        destroy(finish.cur);
    }

    template<typename T, typename Alloc, size_t BufSize>
    void deque<T, Alloc, BufSize>::pop_back() {
        if (finish.cur != finish.first) {
            --finish.cur;
            destroy(finish.cur);
        } else
            pop_back_aux();
    }

    template<typename T, typename Alloc, size_t BufSize>
    void deque<T, Alloc, BufSize>::reallocate_map(size_type nodes_to_add, bool add_at_front) {
        size_type old_num_nodes = finish.node - start.node + 1;
        size_type new_num_nodes = old_num_nodes + nodes_to_add;
        map_pointer new_nstart;
        if (map_size > 2 * new_num_nodes) {
            new_nstart = map + (map_size - new_num_nodes) / 2 + (add_at_front ? nodes_to_add : 0);
            if (new_nstart < start.node)
                std::copy(start.node, finish.node + 1, new_nstart);
            else
                std::copy_backward(start.node, finish.node + 1, new_nstart + old_num_nodes);
        } else {
            size_type new_map_size = map_size + std::max(map_size, nodes_to_add) + 2;
            map_pointer new_map = map_allocator::allocate(new_map_size);
            new_nstart = new_map + (new_map_size - new_num_nodes) / 2 + (add_at_front ? nodes_to_add : 0);
            std::copy(start.node, finish.node + 1, new_nstart);
            map_allocator::deallocate(map, map_size);
            map = new_map;
            map_size = new_map_size;
        }
        start.set_node(new_nstart);
        finish.set_node(new_nstart + old_num_nodes - 1);
    }

    template<typename T, typename Alloc, size_t BufSize>
    void deque<T, Alloc, BufSize>::push_front_aux(const value_type &t) {
        value_type t_copy = t;
        reserve_map_at_front();
        *(start.node - 1) = allocate_node();
        try {
            start.set_node(start.node - 1);
            start.cur = start.last - 1;
            construct(start.cur, t_copy);
        }
        catch (...) {
            start.set_node(start.node + 1);
            start.cur = start.first;
            deallocate_node(*(start.node - 1));
            throw;
        }
    }

    template<typename T, typename Alloc, size_t BufSize>
    void deque<T, Alloc, BufSize>::push_front(const value_type &t) {
        if (start.cur != start.first) {
            construct(start.cur - 1, t);
            --start.cur;
        } else {
            push_front_aux(t);
        }
    }

    template<typename T, typename Alloc, size_t BufSize>
    void deque<T, Alloc, BufSize>::push_back_aux(const value_type &t) {
        value_type t_copy = t;
        reserve_map_at_back();
        *(finish.node + 1) = allocate_node();
        try {
            construct(finish.cur, t_copy);
            finish.set_node(finish.node + 1);
            finish.cur = finish.first;
        }
        catch (...) {
            deallocate_node(*(finish.node + 1));
        }
    }

    template<typename T, typename Alloc, size_t BufSize>
    void deque<T, Alloc, BufSize>::push_back(const value_type &t) {
        if (finish.cur != finish.last - 1) {
            construct(finish.cur, t);
            ++finish.cur;
        } else {
            push_back_aux(t);
        }
    }

    template<typename T, typename Alloc, size_t BufSize>
    void deque<T, Alloc, BufSize>::fill_initialize(deque::size_type n, const value_type &value) {
        create_map_and_node(n);
        map_pointer cur;
        try {
            for (cur = start.node; cur < finish.node; ++cur)
                uninitialized_fill(*cur, *cur + _deque_iterator<T, T &, T *, BufSize>::buffer_size(), value);
            uninitialized_fill(finish.first, finish.cur, value);
        }
        catch (...) {

        }
    }

    template<typename T, typename Alloc, size_t BufSize>
    void deque<T, Alloc, BufSize>::create_map_and_node(deque::size_type num_elements) {
        size_type num_nodes = num_elements / _deque_iterator<T, T &, T *, BufSize>::buffer_size() + 1;
        map_size = std::max(initial_map_size(), num_nodes + 2);
        map = map_allocator::allocate(map_size);
        map_pointer nstart = map + (map_size - num_nodes) / 2;
        map_pointer nfinish = nstart + num_nodes - 1;
        map_pointer cur;
        try {
            for (cur = nstart; cur <= nfinish; ++cur)
                *cur = allocate_node();
        } catch (...) {

        }

        start.set_node(nstart);
        finish.set_node(nfinish);
        start.cur = start.first;
        finish.cur = finish.first + num_elements % _deque_iterator<T, T &, T *, BufSize>::buffer_size();
    }


}


#endif //TINY_STL_DEQUE_HPP
