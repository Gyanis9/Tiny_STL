//
// Created by guo on 24-11-20.
//

#ifndef TINY_STL_TEST_H
#define TINY_STL_TEST_H

#include <iostream>
#include "../include/container/vector.hpp"
#include "../include/container/list.hpp"
#include "../include/container/deque.hpp"
#include "../include/container/stack.hpp"
#include "../include/container/queue.hpp"
#include "../include/container/heap.hpp"

void vector_test() {
    int i;
    std::cout << "-------------------------" << std::endl;
    std::cout << "建立一个vector容器，大小为2，数值为9" << std::endl;
    Tiny::vector<int> iv(2, 9);
    std::cout << "size = " << iv.size() << std::endl;
    std::cout << "capacity = " << iv.capacity() << std::endl;
    for (int j = 0; j < iv.size(); ++j)
        std::cout << iv[j] << " ";
    std::cout << std::endl;
    std::cout << "-------------------------" << std::endl;
    std::cout << "末端插入1" << std::endl;

    iv.push_back(1);
    std::cout << "size = " << iv.size() << std::endl;
    std::cout << "capacity = " << iv.capacity() << std::endl;
    for (int j = 0; j < iv.size(); ++j)
        std::cout << iv[j] << " ";
    std::cout << std::endl;
    std::cout << "-------------------------" << std::endl;
    std::cout << "末端插入2" << std::endl;
    iv.push_back(2);
    std::cout << "size = " << iv.size() << std::endl;
    std::cout << "capacity = " << iv.capacity() << std::endl;
    for (int j = 0; j < iv.size(); ++j)
        std::cout << iv[j] << " ";
    std::cout << std::endl;
    std::cout << "-------------------------" << std::endl;
    std::cout << "末端插入3" << std::endl;
    iv.push_back(3);
    std::cout << "size = " << iv.size() << std::endl;
    std::cout << "capacity = " << iv.capacity() << std::endl;
    for (int j = 0; j < iv.size(); ++j)
        std::cout << iv[j] << " ";
    std::cout << std::endl;
    std::cout << "-------------------------" << std::endl;
    std::cout << "末端插入4" << std::endl;
    iv.push_back(4);
    std::cout << "size = " << iv.size() << std::endl;
    std::cout << "capacity = " << iv.capacity() << std::endl;
    for (int j = 0; j < iv.size(); ++j)
        std::cout << iv[j] << " ";
    std::cout << std::endl;
    std::cout << "-------------------------" << std::endl;
    std::cout << "遍历容器中的数据" << std::endl;
    for (int j = 0; j < iv.size(); ++j)
        std::cout << iv[j] << " ";
    std::cout << std::endl;
    std::cout << "-------------------------" << std::endl;
    std::cout << "末端插入5之后遍历容器中的数字" << std::endl;
    iv.push_back(5);
    for (int j = 0; j < iv.size(); ++j)
        std::cout << iv[j] << " ";
    std::cout << std::endl;
    std::cout << "-------------------------" << std::endl;
    std::cout << "删除两个末端元素并且遍历容器中的数值" << std::endl;
    iv.pop_back();
    iv.pop_back();
    std::cout << "size = " << iv.size() << std::endl;
    std::cout << "capacity = " << iv.capacity() << std::endl;
    for (int j = 0; j < iv.size(); ++j)
        std::cout << iv[j] << " ";
    std::cout << std::endl;
    std::cout << "-------------------------" << std::endl;
    std::cout << "删除一个末端元素并且遍历容器中的数值" << std::endl;
    iv.pop_back();
    std::cout << "size = " << iv.size() << std::endl;
    std::cout << "capacity = " << iv.capacity() << std::endl;
    for (int j = 0; j < iv.size(); ++j)
        std::cout << iv[j] << " ";
    std::cout << std::endl;
    std::cout << "-------------------------" << std::endl;
    std::cout << "查找元素1并在容器中删除它，然后遍历容器中的数值" << std::endl;
    auto ivite = std::find(iv.begin(), iv.end(), 1);
    if (ivite != iv.end())
        iv.erase(ivite);
    std::cout << "size = " << iv.size() << std::endl;
    std::cout << "capacity = " << iv.capacity() << std::endl;
    for (int j = 0; j < iv.size(); ++j)
        std::cout << iv[j] << " ";
    std::cout << std::endl;
    std::cout << "-------------------------" << std::endl;
    std::cout << "查找元素2并在它之前插入三个7，然后遍历容器中的数值" << std::endl;
    ivite = std::find(iv.begin(), iv.end(), 2);
    if (ivite != iv.end())
        iv.insert(ivite, 3, 7);
    std::cout << "size = " << iv.size() << std::endl;
    std::cout << "capacity = " << iv.capacity() << std::endl;
    for (int j = 0; j < iv.size(); ++j)
        std::cout << iv[j] << " ";
    std::cout << std::endl;
    std::cout << "清除容器然后遍历容器" << std::endl;
    iv.clear();
    std::cout << "size = " << iv.size() << std::endl;
    std::cout << "capacity = " << iv.capacity() << std::endl;
    for (int j = 0; j < iv.size(); ++j)
        std::cout << iv[j] << " ";
    std::cout << std::endl;
}

void list_test() {
    std::cout << "-----------------" << std::endl;
    std::cout << "创建一个list对象" << std::endl;
    int i;
    Tiny::list<int> iList;
    std::cout << "size = " << iList.size() << std::endl;

    std::cout << "-----------------" << std::endl;
    std::cout << "依次插入0、1、2、3、4" << std::endl;
    iList.push_back(0);
    iList.push_back(1);
    iList.push_back(2);
    iList.push_back(3);
    iList.push_back(4);
    std::cout << "size = " << iList.size() << std::endl;

    std::cout << "-----------------" << std::endl;
    std::cout << "遍历list容器" << std::endl;
    Tiny::list<int>::iterator ite;
    for (ite = iList.begin(); ite != iList.end(); ++ite)
        std::cout << *ite << " ";
    std::cout << std::endl;
    std::cout << "-----------------" << std::endl;
    std::cout << "查找元素 3 并在其之前插入元素 99 " << std::endl;
    ite = std::find(iList.begin(), iList.end(), 3);
    if (ite != nullptr)
        iList.insert(ite, 99);
    std::cout << "size = " << iList.size() << std::endl;
    std::cout << *ite << std::endl;
    for (ite = iList.begin(); ite != iList.end(); ++ite)
        std::cout << *ite << " ";
    std::cout << std::endl;
    std::cout << "-----------------" << std::endl;
    std::cout << "查找元素 1 并删除" << std::endl;
    ite = std::find(iList.begin(), iList.end(), 1);
    if (ite != nullptr)
        std::cout << *(iList.erase(ite)) << std::endl;
    for (ite = iList.begin(); ite != iList.end(); ++ite)
        std::cout << *ite << " ";
    std::cout << std::endl;
    std::cout << "-----------------" << std::endl;
}


void deque_test() {
    std::cout << "------------------------" << std::endl;
    std::cout << "创建一个deque对象" << std::endl;
    Tiny::deque<int, Tiny::alloc, 8> ideq(20, 9);
    std::cout << "size = " << ideq.size() << std::endl;
    for (int i = 0; i < ideq.size(); ++i)
        std::cout << ideq[i] << " ";
    std::cout << std::endl;
    std::cout << "------------------------" << std::endl;
    std::cout << "为每个元素设置新值" << std::endl;
    for (int i = 0; i < ideq.size(); ++i)
        ideq[i] = i;
    for (int i = 0; i < ideq.size(); ++i)
        std::cout << ideq[i] << " ";
    std::cout << std::endl;
    std::cout << "------------------------" << std::endl;
    std::cout << "尾端插入三个元素，其值为0，1，2" << std::endl;
    for (int i = 0; i < 3; ++i)
        ideq.push_back(i);
    std::cout << "size = " << ideq.size() << std::endl;
    for (int i = 0; i < ideq.size(); ++i)
        std::cout << ideq[i] << " ";
    std::cout << std::endl;
    std::cout << "------------------------" << std::endl;
    std::cout << "尾端插入三个元素，其值为3" << std::endl;
    ideq.push_back(3);
    std::cout << "size = " << ideq.size() << std::endl;
    for (int i = 0; i < ideq.size(); ++i)
        std::cout << ideq[i] << " ";
    std::cout << std::endl;
    std::cout << "------------------------" << std::endl;
    std::cout << "前端插入1个元素，其值为99" << std::endl;
    ideq.push_front(99);
    std::cout << "size = " << ideq.size() << std::endl;
    for (int i = 0; i < ideq.size(); ++i)
        std::cout << ideq[i] << " ";
    std::cout << std::endl;
    std::cout << "------------------------" << std::endl;
    std::cout << "前端插入2个元素，其值为98,97" << std::endl;
    ideq.push_front(98);
    ideq.push_front(97);
    std::cout << "size = " << ideq.size() << std::endl;
    for (int i = 0; i < ideq.size(); ++i)
        std::cout << ideq[i] << " ";
    std::cout << std::endl;
    std::cout << "------------------------" << std::endl;
    std::cout << "查找一个元素99" << std::endl;
    Tiny::deque<int, Tiny::alloc, 8>::iterator itr;
    itr = std::find(ideq.begin(), ideq.end(), 99);
    std::cout << *itr << std::endl;
    std::cout << *(itr.cur) << std::endl;
    std::cout << "size = " << ideq.size() << std::endl;
    for (int i = 0; i < ideq.size(); ++i)
        std::cout << ideq[i] << " ";
    std::cout << std::endl;
    std::cout << "------------------------" << std::endl;
}


void stack_test() {
    std::cout << "---------------------" << std::endl;
    std::cout << "创建stack对象并插入1、3、5、7" << std::endl;
    Tiny::stack<int, Tiny::list<int>> istack;
    istack.push(1);
    istack.push(3);
    istack.push(5);
    istack.push(7);
    std::cout << "---------------------" << std::endl;
    std::cout << "查询stack大小以及top元素" << std::endl;
    std::cout << istack.size() << std::endl;
    std::cout << istack.top() << std::endl;
    std::cout << "---------------------" << std::endl;
    std::cout << "依次退栈并查询top元素" << std::endl;
    istack.pop();
    std::cout << istack.top() << std::endl;
    istack.pop();
    std::cout << istack.top() << std::endl;
    istack.pop();
    std::cout << istack.top() << std::endl;
    std::cout << istack.size() << std::endl;
}

void queue_test() {
    std::cout << "----------------------------" << std::endl;
    std::cout << "创建一个queue对象并插入1、3、5、7" << std::endl;
    Tiny::queue<int, Tiny::list<int>> iqueue;
    iqueue.push(1);
    iqueue.push(3);
    iqueue.push(5);
    iqueue.push(7);
    std::cout << "----------------------------" << std::endl;
    std::cout << "查询queue大小以及队头元素" << std::endl;
    std::cout << "size = " << iqueue.size() << std::endl;
    std::cout << "front = " << iqueue.front() << std::endl;
    std::cout << "----------------------------" << std::endl;
    std::cout << "依次出队并查询队头元素" << std::endl;
    iqueue.pop();
    std::cout << "front = " << iqueue.front() << std::endl;
    iqueue.pop();
    std::cout << "front = " << iqueue.front() << std::endl;
    iqueue.pop();
    std::cout << "front = " << iqueue.front() << std::endl;
    std::cout << "----------------------------" << std::endl;
    std::cout << "查询队伍大小" << std::endl;
    std::cout << "size = " << iqueue.size() << std::endl;
}

#include <vector>

void heap_test() {
    int ia[9] = {0, 1, 2, 3, 4, 8, 9, 3, 5};
    std::vector<int> ivec(ia, ia + 9);
    std::cout << "-------------------------------------------" << std::endl;
    std::cout << "创建堆并打印" << std::endl;
    Tiny::make_heap(ivec.begin(), ivec.end());
    for (const auto &x: ivec)
        std::cout << x << " ";
    std::cout << std::endl;
    std::cout << "-------------------------------------------" << std::endl;
    std::cout << "插入元素7" << std::endl;
    ivec.push_back(7);
    Tiny::push_heap(ivec.begin(), ivec.end());
    for (const auto &x: ivec)
        std::cout << x << " ";
    std::cout << std::endl;
    std::cout << "-------------------------------------------" << std::endl;
    std::cout << "-------------------------------------------" << std::endl;
    ivec.push_back(7);
    Tiny::pop_heap(ivec.begin(), ivec.end());
    std::cout << ivec.back() << std::endl;
    ivec.pop_back();
    for (const auto &x: ivec)
        std::cout << x << " ";
    std::cout << std::endl;
    std::cout << "-------------------------------------------" << std::endl;
    std::cout << "-------------------------------------------" << std::endl;
//    Tiny::sort_heap(ivec.begin(), ivec.end());
    for (const auto &x: ivec)
        std::cout << x << " ";
    std::cout << std::endl;
}


#endif //TINY_STL_TEST_H
