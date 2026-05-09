#include <iostream>
using namespace std;

int main() {
    //根据数字输出星期几（int 类型）
    int day;
    cout << "[示例1] 请输入数字1-7（1=周一，7=周日）：";
    cin >> day;

    switch (day) {
        case 1:
            cout << "周一" << endl;
            break;
        case 2:
            cout << "周二" << endl;
            break;
        case 3:
            cout << "周三" << endl;
            break;
        case 4:
            cout << "周四" << endl;
            break;
        case 5:
            cout << "周五" << endl;
            break;
        case 6:
            cout << "周六" << endl;
            break;
        case 7:
            cout << "周日" << endl;
            break;
        default:
            cout << "无效输入，请输入1-7之间的数字" << endl;
            break;
    }

    //根据运算符进行简单计算（char 类型）

       {double a, b;
        char op;
        cout << "[示例2] 请输入两个数（空格分隔）：";
        cin >> a >> b;
        cout << "请输入运算符(+ - * /)：";
        cin >> op;
        switch (op) {
            case '+': cout << a << " + " << b << " = " << a + b << endl; break;
            case '-': cout << a << " - " << b << " = " << a - b << endl; break;
            case '*': cout << a << " * " << b << " = " << a * b << endl; break;
            case '/':
                if (b != 0)
                    cout << a << " / " << b << " = " << a / b << endl;
                else
                    cout << "除数不能为0" << endl;
                break;
            default: cout << "无效运算符" << endl; break;}
        }
    //月份天数

    {int month;
        cout << "[示例3] 请输入月份（1-12）：";
        cin >> month;
        int days;
        switch (month) {
            case 1: case 3: case 5: case 7: case 8: case 10: case 12:
                days = 31; break;
            case 4: case 6: case 9: case 11:
                days = 30; break;
            case 2:
                days = 28; break;
            default:
                days = -1; break;
        }
        if (days == -1)
            cout << "无效月份" << endl;
        else
            cout << month << "月有 " << days << " 天" << endl; }
            return 0;
}
