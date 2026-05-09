#include <iostream>
#include "calculator.h"

int main() {
    int x = 10, y = 5;
    std::cout << "加法: " << add(x, y) << std::endl;
    std::cout << "减法: " << subtract(x, y) << std::endl;
    return 0;
}
