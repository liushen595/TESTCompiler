#include <iostream>
#include <string>
#ifdef _WIN32
#include <windows.h>
#endif
#include "TESTscan.hpp"

// 声明词法分析和语法分析入口函数
int doScan(const std::string &filename);
void doParse();

int main(int argc, char *argv[]) {
#ifdef _WIN32
    // 防止控制台中文乱码
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
    std::cout << "TEST compiler start running...\n";
    std::string inputfile;
    if (argc > 1) {
        inputfile = argv[1];
    }
    else {
        std::cout << "No input file, please enter: ";
        std::getline(std::cin, inputfile);
    }
    if (doScan(inputfile)) {
        return 1;
    }
    doParse();
    std::cout << "Done.\n";
    return 0;
}
