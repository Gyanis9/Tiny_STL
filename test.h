//
// Created by guo on 24-11-20.
//

#ifndef TINY_STL_TEST_H
#define TINY_STL_TEST_H

#include "sequence_container/lib/vector.hpp"
#include <iostream>


void vector_test() {
    int i;
    std::cout << "-------------------------" << std::endl;
    std::cout << "建立一个vector容器，大小为2，数值为9" << std::endl;
    Gyanis::vector<int> iv(2, 9);
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


#endif //TINY_STL_TEST_H
