//
// Created by guo on 24-11-29.
//

#ifndef TINY_STL_STACK_HPP
#define TINY_STL_STACK_HPP

#include "../../iterator/lib/iterator.hpp"
#include "deque.hpp"
#include "list.hpp"

namespace Gyanis {
    template<typename T, typename Sequence=deque<T>>
    class stack {
        friend bool operator==(const stack &, const stack &);

        friend bool operator<(const stack &, const stack &);

    public:
        typedef typename Sequence::value_type value_type;
        typedef typename Sequence::size_type size_type;
        typedef typename Sequence::reference reference;
        typedef typename Sequence::const_reference const_reference;

    protected:
        Sequence c;

    public:
        bool empty() const { return c.empty(); }

        size_type size() const { return c.size(); }

        reference top() { return c.back(); }

        const_reference top() const { return c.back(); }

        void push(const value_type &x) { c.push_back(x); }

        void pop() { c.pop_back(); }
    };

    template<typename T, typename Sequence>
    bool operator==(const stack<T, Sequence> &x, const stack<T, Sequence> &y) {
        return x.c == y.c;
    }

    template<typename T, typename Sequence>
    bool operator<(const stack<T, Sequence> &x, const stack<T, Sequence> &y) {
        return x.c < y.c;
    }
}


#endif //TINY_STL_STACK_HPP
