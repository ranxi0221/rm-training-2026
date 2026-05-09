#include <iostream>
using namespace std;

int main() {
    // do-while 示例1：密码验证 
    {
        int password;
        cout << "[do-while示例1] 密码验证（预设密码为 123）" << endl;
        do {
            cout << "请输入密码：";
            cin >> password;
            if (password != 123) {
                cout << "密码错误，请重试。" << endl;
            }
        } while (password != 123);
        cout << "密码正确！欢迎进入系统。" << endl;
    }

    // do-while 示例2：菜单交互 
    {
        int choice;
        do {
            cout << "\n[do-while示例2] 菜单" << endl;
            cout << "1. 开始游戏" << endl;
            cout << "2. 查看积分" << endl;
            cout << "3. 退出" << endl;
            cout << "请选择（1-3）：";
            cin >> choice;
            switch (choice) {
                case 1: cout << "游戏开始..." << endl; break;
                case 2: cout << "当前积分：100" << endl; break;
                case 3: cout << "正在退出..." << endl; break;
                default: cout << "无效选项，请重新选择" << endl; break;
            }
        } while (choice != 3);
    }

    //  do-while 示例3：累加求和直到输入0
    {
        int num, sum = 0;
        cout << "\n[do-while示例3] 请输入整数（输入0结束累加）" << endl;
        do {
            cout << "请输入一个整数：";
            cin >> num;
            sum += num;   // 即使是0也会加上，但加上0不影响总和
        } while (num != 0);
        cout << "所有数的总和为：" << sum << endl;
    }

    return 0;
}