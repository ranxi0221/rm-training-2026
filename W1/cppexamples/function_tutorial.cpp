/*/      函数学习
 * 演示 C++ 中函数的各种用法，包括：
 *       1. 函数定义与调用
 *       2. 值传递与引用传递
 *       3. 默认参数
 *       4. 函数重载
 *       5. 函数返回值
 */

#include <iostream>
using namespace std;

// 1. 函数定义与调用
// 定义一个简单的函数，返回两个整数的最大值，max函数接收两个int,返回较大的那个，printmessage没有返回数值只打印信息
//调用：直接写函数名加括号，传入实际参数。
int max(int a, int b) {
    if (a > b)
        return a;
    else
        return b;
}

// 无返回值(void)的函数，只打印信息
void printMessage(string msg) {
    cout << "消息: " << msg << endl;
}

// 2. 值传递 vs 引用传递 
// 值传递：传递的是参数的副本，函数内修改不影响原始变量：changeByValue(int x)中的x是实参的一个副本。函数内修改 x 不会影响外部的变量。
void changeByValue(int x) {
    x = 100;  // 修改的是副本，外部实参不受影响
    cout << "   [changeByValue] 函数内部 x = " << x << endl;
}

// 引用传递：传递的是变量本身，函数内修改会改变原始变量:changeByReference(int &x) 中的 & 表示引用，别名指向原始变量。函数内修改 x 会直接改变外部变量。


void changeByReference(int &x) {
    x = 100;  // 直接修改原始变量
    cout << "   [changeByReference] 函数内部 x = " << x << endl;
}

// 3. 默认参数 
// 带默认参数的函数，调用时可以省略最后一个参数
//在函数声明或定义时，为参数指定默认值：string prefix = "Hello"。
//注意：默认参数必须从右向左连续，即一旦某个参数有默认值，其右侧所有参数都应有默认值。
void greet(string name, string prefix = "Hello") {
    cout << prefix << ", " << name << "!" << endl;
}

// 4. 函数重载 
// 同名函数，参数列表不同（参数个数或类型不同）：同一作用域内，函数名相同，但参数列表不同（参数个数、类型或顺序不同）。
//返回值类型不能作为区分重载的依据。编译器根据调用时传入的参数类型和个数自动选择匹配的重载版本。
//示例中 add 有三个重载：两个整数、两个浮点数、三个整数。
int add(int a, int b) {
    return a + b;
}

double add(double a, double b) {
    return a + b;
}

int add(int a, int b, int c) {
    return a + b + c;
}

// 5. 返回值与作用域 
// 函数可以返回局部变量的值（不是引用或指针时安全）：函数内定义的局部变量在函数结束时销毁，但返回值是将值拷贝回调用者，所以安全。
//绝对不要返回局部变量的引用或指针，因为那块内存会被回收，导致未定义行为。

int square(int x) {
    int result = x * x;   // 局部变量，在函数结束时销毁
    return result;        // 返回的是值的拷贝，安全
}

// 警告：不要返回局部变量的引用或指针（此处仅为说明）
// int& badReturn() { int local = 5; return local; } // 错误！

// 主函数 
int main() {
    cout << " 1. 函数定义与调用 " << endl;
    int a = 10, b = 20;
    int m = max(a, b);
    cout << "max(" << a << ", " << b << ") = " << m << endl;
    printMessage("C++ 函数学习");

    cout << "\n 2. 值传递 vs 引用传递 " << endl;
    int value = 5;
    cout << "调用前 value = " << value << endl;
    changeByValue(value);
    cout << "调用 changeByValue 后 value = " << value << " (未改变)" << endl;
    changeByReference(value);
    cout << "调用 changeByReference 后 value = " << value << " (已改变)" << endl;

    cout << "\n3. 默认参数 " << endl;
    greet("张三");               // 使用默认的 "Hello"
    greet("李四", "Hi");        // 覆盖默认参数

    cout << "\n4. 函数重载 " << endl;
    cout << "add(3,5) = " << add(3, 5) << endl;
    cout << "add(3.14, 2.56) = " << add(3.14, 2.56) << endl;
    cout << "add(1,2,3) = " << add(1, 2, 3) << endl;

    cout << "\n5. 返回值与作用域 " << endl;
    int sq = square(9);
    cout << "square(9) = " << sq << endl;

    return 0;
}