/**
 * 文件: reference_tutorial.cpp
 * 演示 C++ 中引用的各种用法，包括：
 *       1. 引用的基本语法（别名）
 *       2. 引用作为函数参数（实现输出参数或高效传递）
 *       3. 引用作为函数返回值（谨慎使用）
 *       4. const 引用（常引用）
 *       5. 引用的常见误区
 */

#include <iostream>
#include <string>
using namespace std;

// 1. 引用的基本语法 ：定义格式：类型 &引用名 = 原变量;
//& 符号出现在变量名前面表示“引用类型”，不是取地址（取地址的 & 出现在表达式中）。 引用必须在声明时初始化，且不能重新绑定到另一个变量。
//对引用的所有操作，都会直接作用在原变量上。
void basicReference() {
    cout << "\n1. 引用的基本语法 " << endl;
    int original = 10;
    int &ref = original;   // ref 是 original 的引用（别名）
    
    cout << "original = " << original << endl;
    cout << "ref = " << ref << endl;
    
    // 通过引用修改值
    ref = 20;
    cout << "执行 ref = 20 后：" << endl;
    cout << "original = " << original << endl;
    cout << "ref = " << ref << endl;
    
    // 注意：引用必须在声明时初始化，且不能重新绑定到其他变量
    // int &ref2;   // 错误：引用必须初始化
    // int &ref2 = original;  // 正确
}

// 2. 引用作为函数参数 
// 值传递：无法修改实参
//需要函数内部修改外部变量时（输出参数、交换、赋值等）。避免拷贝大数据（如大型结构体、类对象），提高效率。
void swapByValue(int a, int b) {
    int t = a;
    a = b;
    b = t;
}

// 引用传递：可以直接修改实参
void swapByReference(int &a, int &b) {
    int t = a;
    a = b;
    b = t;
}

void referenceAsParam() {
    cout << "\n2. 引用作为函数参数 " << endl;
    int x = 10, y = 20;
    cout << "初始值: x = " << x << ", y = " << y << endl;
    
    swapByValue(x, y);
    cout << "调用 swapByValue 后: x = " << x << ", y = " << y << " (未改变)" << endl;
    
    swapByReference(x, y);
    cout << "调用 swapByReference 后: x = " << x << ", y = " << y << " (已交换)" << endl;
}

//  3. 引用作为函数返回值 
// 返回引用时，函数返回值可以作为左值（即可以放在赋值号左边被修改）。
//绝对不要返回局部变量的引用，因为局部变量在函数结束时销毁，返回的引用会悬空（dangling reference），导致未定义行为。
//可以返回静态变量、全局变量、类成员等生命周期比函数长的变量。
int global = 100;
int& returnGlobalRef() {
    return global;   // 返回 global 的引用
}

// 警告：不要返回局部变量的引用（错误示例，注释掉）
// int& badReturn() {
//     int local = 5;
//     return local;   // 错误！local 在函数结束后销毁
// }

void referenceAsReturn() {
    cout << "\n3. 引用作为函数返回值 " << endl;
    cout << "初始 global = " << global << endl;
    
    int &ref = returnGlobalRef();   // ref 是 global 的别名
    ref = 200;
    cout << "通过返回的引用修改后，global = " << global << endl;
}

// 4. const 引用（常引用）
// 常引用可以绑定到常量、字面量、临时对象，延长其生命周期
//用 const 修饰的引用，不能通过该引用修改所绑定的对象。常引用可以绑定到常量、字面量、临时对象，延长临时量的生命周期至引用作用域结束。 
//将函数参数设为 const & 是一种常见优化：既避免拷贝（尤其是大对象），又保护原始数据不被修改。
void printConstRef(const int &x) {
    // x = 100;   // 错误：不能通过常引用修改值
    cout << "常引用接收的值: " << x << endl;
}

void constReference() {
    cout << "\n 4. const 引用" << endl;
    int a = 10;
    const int &ref = a;   // 可以绑定到变量，但不能通过 ref 修改 a
    // ref = 20;   // 错误
    
    // 常引用可以绑定到字面量
    const int &r = 100;   // 合法：常引用会延长临时量的生命周期
    cout << "绑定字面量的常引用: " << r << endl;
    
    // 函数参数使用常引用，避免拷贝又能保护数据
    printConstRef(a);
    printConstRef(250);    // 字面量也能传
}

// 5. 引用的常见误区 
void commonMistakes() {
    cout << "\n 5. 引用的常见误区" << endl;
    
    // 误区1：引用不是指针，不需要取地址
    int val = 5;
    int &ref = val;
    cout << "val = " << val << ", ref = " << ref << endl;
    
    // 误区2：引用一旦初始化，不能改变指向（即不能成为另一个变量的别名）
    int another = 10;
    // ref = another;   // 这不是让 ref 引用 another，而是把 another 的值赋给 ref（即修改了 val）
    cout << "执行 ref = another 后，ref 仍引用 val，val 的值变为 " << val << endl;
    
    // 误区3：不要返回局部变量的引用（前面已说明）
}

int main() {
    cout << " C++ 引用教程 " << endl;
    basicReference();
    referenceAsParam();
    referenceAsReturn();
    constReference();
    commonMistakes();
    return 0;
}