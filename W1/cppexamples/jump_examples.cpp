#include <iostream>
using namespace std;

int main() {
    //  示例1：break 语句（提前退出循环）
    {
        cout << "[break示例] 输入一个整数，程序会从1开始累加，当和超过该整数时停止并输出和。\n";
        int limit, sum = 0;
        cout << "请输入一个正整数：";
        cin >> limit;
        for (int i = 1; ; i++) {   // 无限循环，靠 break 退出
            sum += i;
            if (sum > limit) {
                cout << "加到 " << i << " 时，累积和 " << sum << " 已经超过 " << limit << endl;
                break;  // 立刻跳出循环
            }
        }
    }

    // 示例2：continue 语句（跳过本次循环剩余部分） 
    {
        cout << "\n[continue示例] 输出1到10之间的奇数（跳过偶数）。\n";
        for (int i = 1; i <= 10; i++) {
            if (i % 2 == 0) {
                continue;   // 跳过偶数，不执行下面的输出
            }
            cout << i << " ";
        }
        cout << endl;
    }

    // 示例3：goto 语句（无条件跳转）
    {
        cout << "\n[goto示例] 模拟错误处理：输入一个负数会跳转到错误处理代码。\n";
        int num;
        cout << "请输入一个整数：";
        cin >> num;
        if (num < 0) {
            goto error;   // 跳过正常流程，直接跳转到 error 标签
        }
        cout << "你输入的是正数或零：" << num << endl;
        goto normal_end;  // 跳过错误处理部分

    error:
        cout << "错误：不能输入负数！" << endl;
        // 这里可以写清理代码

    normal_end:
        cout << "程序继续执行（goto 示例结束）。" << endl;
    }

    return 0;
}