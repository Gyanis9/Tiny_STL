#include <iostream>

int main() {
    int* p1 = nullptr;
    int* p2 = nullptr;

    auto diff = p1 - p2; // 未定义行为
    std::cout << "Difference: " << diff << std::endl;

    return 0;
}
