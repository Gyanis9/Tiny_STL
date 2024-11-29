//
// Created by guo on 24-11-25.
//

#ifndef TINY_STL_LIST_HPP
#define TINY_STL_LIST_HPP

#include "../../allocator/lib/allocator.hpp"


namespace Gyanis {
    /**节点*/
    template<typename T>
    struct _list_node {
        typedef void *void_pointer;
        _list_node<T> *prev;
        _list_node<T> *next;
        T data;
    };

    /**链表的迭代器*/
    template<typename T, typename Ref, typename Ptr>
    struct _list_iterator {
        typedef _list_iterator<T, T &, T *> iterator;
        typedef _list_iterator<T, const T &, const T *> const_iterator;
        typedef _list_iterator<T, Ref, Ptr> self;

        typedef bidirectional_iterator_tag iterator_category;
        typedef T value_type;
        typedef Ptr pointer;
        typedef Ref reference;
        typedef _list_node<value_type> *link_type;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;

        link_type node;

        /**构造函数*/
        _list_iterator(link_type x) : node(x) {}

        _list_iterator() {}

        _list_iterator(const iterator &x) : node(x.node) {}

        /**运算符重载*/
        bool operator==(const self &x) const { return node == x.node; }

        bool operator!=(const self &x) const { return node != x.node; }

        reference operator*() const { return node->data; }

        pointer operator->() const { return &(operator*()); }

        self &operator++() {
            node = static_cast<link_type >(node->next);
            return *this;
        }

        self operator++(int) {
            self temp = *this;
            ++*this;
            return temp;
        }

        self &operator--() {
            node = static_cast<link_type>((*node).prev);
            return *this;
        }

        self operator--(int) {
            self temp = *this;
            --*this;
            return temp;
        }
    };

    template<typename T, typename Alloc=alloc>
    class list {
        /**基本数据重命名*/
    public:
        typedef T value_type;
        typedef size_t size_type;
        typedef value_type &reference;
        typedef const value_type &const_reference;
        typedef value_type *pointer;
    protected:
        typedef _list_node<value_type> list_node;
        typedef simple_alloc<list_node, Alloc> list_node_allocator;
    public:
        typedef list_node *link_type;
        typedef _list_iterator<value_type, reference, pointer> iterator;
        typedef _list_iterator<const value_type, reference, const pointer> const_iterator;
    protected:
        link_type node;

        link_type get_node() { return list_node_allocator::allocate(); };

        void put_node(link_type p) { list_node_allocator::deallocate(p); }

        link_type create_node(const T &x) {
            link_type result = get_node();
            construct(&result->data, x);
            return result;
        }

        void destroy_node(link_type p) {
            destroy(&p->data);
            put_node(p);
        }

        void empty_initialize() {
            node = get_node();
            node->prev = node;
            node->next = node;
        }


        /**将[first,last)内的所有元素移动到position之前*/
        void transfer(iterator position, iterator first, iterator last) {
            if (position != last) {
                (static_cast<link_type>((*last.node).prev))->next = position.node;
                (static_cast<link_type>((*first.node).prev))->next = last.node;
                (static_cast<link_type>((*position.node).prev))->next = first.node;
                link_type temp = static_cast<link_type >(*position.node)->prev;
                position.node->prev = last.node->prev;
                last.node->prev = first.node->prev;
                first.node->prev = temp;
            }

        }

    public:
        list() { empty_initialize(); }


        iterator begin() { return static_cast<link_type >(node->next); }

        iterator begin() const { return static_cast<link_type >(node->next); }

        iterator end() { return node; }

        iterator end() const { return node; }

        bool empty() const { return node->next == node; }

        size_type size() const {
            size_type result = 0;
            result = distance(begin(), end());
            return result;
        }

        iterator insert(iterator position, const T &x) {
            link_type temp = create_node(x);
            temp->next = position.node;
            temp->prev = position.node->prev;
            (static_cast<link_type>(position.node->prev))->next = temp;
            position.node->prev = temp;
            return temp;
        }

        reference front() { return *begin(); }

        reference back() { return *(--end()); }

        void push_front(const T &x) { insert(begin(), x); }

        void push_back(const T &x) { insert(end(), x); }

        iterator erase(iterator position) {
            link_type next_node = static_cast<link_type >(position.node->next);
            link_type prev_node = static_cast<link_type >(position.node->prev);
            prev_node->next = next_node;
            next_node->prev = prev_node;
            destroy_node(position.node);
            return iterator(next_node);
        }

        void pop_front() { erase(begin()); }

        void pop_back() {
            auto temp = end();
            erase(--temp);
        }

        void clear();

        void remove(const T &x);

        void unique();

        void splice(iterator position, list &x);

        void splice(iterator position, list &x, iterator i);

        void splice(iterator position, list &x, iterator fist, iterator last);

        void merge(list<T, Alloc> &x);

        void reverse();

        void sort();

    };

    template<typename T, typename Alloc>
    void list<T, Alloc>::sort() {
        if (node->next == node || static_cast<link_type>(node->next)->next == node)
            return;

    }

    template<typename T, typename Alloc>
    void list<T, Alloc>::reverse() {
        if (node->next == node || static_cast<link_type>(node->next)->next == node)
            return;
        auto first = begin();
        ++first;
        while (first != end()) {
            auto old = first;
            ++first;
            transfer(begin(), old, first);
        }
    }

    template<typename T, typename Alloc>
    void list<T, Alloc>::merge(list<T, Alloc> &x) {
        auto first1 = begin();
        auto last1 = end();
        auto first2 = x.begin();
        auto last2 = x.end();
        while (first1 != last1 && first2 != last2) {
            if (*first2 < *first1) {
                auto next = first2;
                transfer(first1, first2, ++next);
                first2 = next;
            } else
                ++first1;
        }
        if (first2 != last2)
            transfer(last1, first2, last2);
    }

    template<typename T, typename Alloc>
    void list<T, Alloc>::splice(list::iterator position, list &x, list::iterator first, list::iterator last) {
        if (last != first)
            transfer(position, first, last);
    }

    template<typename T, typename Alloc>
    void list<T, Alloc>::splice(list::iterator position, list &x, list::iterator i) {
        iterator j = i;
        ++j;
        if (position == i || position == j)
            return;
        transfer(position, i, j);
    }

    template<typename T, typename Alloc>
    void list<T, Alloc>::splice(list::iterator position, list &x) {
        if (!x.empty())
            transfer(position, x.begin(), x.end());
    }

    template<typename T, typename Alloc>
    void list<T, Alloc>::unique() {
        auto first = begin();
        auto last = end();
        if (first == last)
            return;
        auto next = first;
        while (++next != last) {
            if (*first == *next)
                erase(next);
            else
                first = next;
            next = first;
        }
    }

    template<typename T, typename Alloc>
    void list<T, Alloc>::remove(const T &x) {
        auto first = begin();
        auto last = end();
        while (first != last) {
            auto next = first;
            ++next;
            if (*first == x)
                erase(first);
            first = next;
        }
    }

    template<typename T, typename Alloc>
    void list<T, Alloc>::clear() {
        link_type cur = static_cast<link_type >(node->next);
        while (cur != node) {
            link_type temp = cur;
            cur = static_cast<link_type >(cur->next);
            destroy_node(temp);
        }
        node->next = node;
        node->prev = node;
    }
}

#endif //TINY_STL_LIST_HPP