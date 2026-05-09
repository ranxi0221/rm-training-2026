#include <iostream>
using namespace std;

int main() {
    // for 循环示例1：累加求和 
    {
        int n;
        cout << "[for示例1] 请输入一个正整数 N（将计算 1+2+...+N）：";
        cin >> n;
        int sum = 0;
        for (int i = 1; i <= n; i++) {
            sum += i;   // 等价于 sum = sum + i
        }
        cout << "1 到 " << n << " 的和是：" << sum << endl;
    }

    // for 循环示例2：输出部分乘法表
    {
        int num;
        cout << "\n[for示例2] 请输入一个整数（将输出它的乘法表，从1乘到10）：";
        cin >> num;
        for (int j = 1; j <= 10; j++) {
            cout << num << " × " << j << " = " << num * j << endl;
        }
    }

    //  while 循环示例1：倒序输出 
    {
        int start;
        cout << "\n[while示例1] 请输入一个正整数 M（将倒序输出 M, M-1, ..., 1）：";
        cin >> start;
        int i = start;
        cout << "倒序输出：";
        while (i >= 1) {
            cout << i << " ";
            i--;
        }
        cout << endl;
    }

    // while 循环示例2：数字反转（如输入1234，输出4321）
    {
        int num, reversed = 0;
        cout << "\n[while示例2] 请输入一个整数（将输出它的数字反转）：";
        cin >> num;
        int original = num;  // 保留原值，最后输出用
        while (num != 0) {
            reversed = reversed * 10 + num % 10;   // 取出最后一位，并加到反转结果上
            num /= 10;                            // 去掉最后一位
        }
        cout << original << " 反转后是：" << reversed << endl;
    }

    return 0;
}