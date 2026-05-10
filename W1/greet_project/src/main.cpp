#include <iostream>
#include <cstring>
#include "greet.h"
using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "用法: ./greet [-h] 名字1 [名字2 ...]" << endl;
        return 1;
    }
    if (strcmp(argv[1], "-h") == 0) {
        printHelp();
        return 0;
    }
    greetMultiple(argc, argv);
    return 0;
}