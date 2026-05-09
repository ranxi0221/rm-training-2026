/**
 *  class_tutorial.cpp
 *  演示 C++ 中简单类的定义和使用
 * 内容:
 *   1. 类的定义与对象创建
 *   2. 公有成员 (public) 与私有成员 (private)
 *   3. 成员函数（方法）的定义与调用
 *   4. 构造函数与析构函数
 *   5. 封装性的简单示例
 */

#include <iostream>
#include <string>
using namespace std;

// 1. 最简单的类 
// 定义一个表示“人”的类
class Person {
public:   // 访问说明符，表示其后的成员可以在类外部访问。
    string name;   // 成员变量（属性）描述对象的状态。
    int age;
    
    // 成员函数（方法）：自我介绍，定义对象的行为。
    void introduce() {
        cout << "我叫" << name << "，今年" << age << "岁。" << endl;
    }
};

//  2. 带私有成员的类与封装 
class BankAccount {
private:  // 私有成员：只能被类内部的函数访问
    double balance;   // 余额（外部不能直接修改）将数据（balance）隐藏在类内部，只通过公有的成员函数来间接操作。这可以保证数据的安全性
    
public:
    // 构造函数：创建对象时自动调用，用于初始化
    BankAccount(double initial) {
        if (initial < 0) {
            balance = 0;
            cout << "初始金额不能为负，已设为0。" << endl;
        } else {
            balance = initial;
        }
    }
    
    // 存款
    void deposit(double amount) {
        if (amount > 0) {
            balance += amount;
            cout << "存款 " << amount << " 元成功。" << endl;
        } else {
            cout << "存款金额无效。" << endl;
        }
    }
    
    // 取款
    void withdraw(double amount) {
        if (amount > 0 && amount <= balance) {
            balance -= amount;
            cout << "取款 " << amount << " 元成功。" << endl;
        } else {
            cout << "余额不足或金额无效。" << endl;
        }
    }
    
    // 查询余额（只读）
    double getBalance() {
        return balance;
    }
};

// 3. 构造函数与析构函数示例 
class Message {
private:
    string text;
    
public:
    // 构造函数：函数名与类名相同，无返回值。可以重载（多个参数版本）。在对象创建时自动调用。
    Message() {
        text = "默认消息";
        cout << "构造函数（无参）被调用，text = " << text << endl;
    }
    
    Message(string msg) {
        text = msg;
        cout << "构造函数（有参）被调用，text = " << text << endl;
    }
    
    // 析构函数：函数名是 ~类名，无参数、无返回值。在对象销毁时自动调用，通常用于释放资源（如动态内存、文件句柄等）。
    ~Message() {
        cout << "析构函数被调用，清理消息：" << text << endl;
    }
    
    void show() {
        cout << "消息内容：" << text << endl;
    }
};

// 主函数
int main() {
    cout << " 1. 简单类 Person " << endl;
    // 创建对象（实例化）
    Person p1;
    p1.name = "张三";
    p1.age = 20;
    p1.introduce();
    
    Person p2;
    p2.name = "李四";
    p2.age = 22;
    p2.introduce();
    
    cout << "\n2. 封装与私有成员 BankAccount" << endl;
    BankAccount myAccount(1000);   // 创建对象并传入初始金额
    myAccount.deposit(500);
    myAccount.withdraw(200);
    cout << "当前余额：" << myAccount.getBalance() << " 元" << endl;
    // myAccount.balance = 9999;   // 错误！因为 balance 是私有成员，外部无法访问
    
    cout << "\n 3. 构造函数与析构函数 " << endl;
    Message msg1;               // 调用无参构造函数
    msg1.show();
    
    Message msg2("你好，C++");  // 调用有参构造函数
    msg2.show();
    
    // 当程序结束时，msg1 和 msg2 的析构函数会自动被调用
    return 0;
}