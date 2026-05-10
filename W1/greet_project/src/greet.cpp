#include <iostream>
#include <cstring>
#include "greet.h"
using namespace std;

void printHelp() {
    cout << "打招呼程序\n用法: ./greet 名字1 [名字2 ...]\n选项: -h 显示此帮助" << endl;
}

void greetMultiple(int argc, char* argv[]) {
    for (int i = 1; i < argc; ++i) {
        cout << "Hello " << argv[i] << "!" << endl;   // 注意：argv[i] 是 char*，可以直接输出
    }
}