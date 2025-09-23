#include <iostream>
#include <fstream>
#include <string>

// 引入词法分析器头文件
#ifdef LEXER_ENABLED
#include "Lexer.hpp"
#endif

int main(int argc, char* argv[]) {
    std::cout << "TESTCompiler - 编译原理课程编译器" << std::endl;

    if (argc < 2) {
        std::cout << "使用方法: " << argv[0] << " <输入文件>" << std::endl;
        return 1;
    }

    std::string inputFile = argv[1];
    std::cout << "正在处理文件: " << inputFile << std::endl;

    // 检查文件是否存在
    std::ifstream file(inputFile);
    if (!file.is_open()) {
        std::cerr << "错误: 无法打开文件 " << inputFile << std::endl;
        return 1;
    }

    // 读取文件内容
    std::string content;
    std::string line;
    while (std::getline(file, line)) {
        content += line + "\n";
    }
    file.close();

    std::cout << "文件内容读取成功，大小: " << content.size() << " 字节" << std::endl;

#ifdef LEXER_ENABLED
    // TODO: 词法分析
    std::cout << "开始词法分析..." << std::endl;
    // Lexer lexer(content);
    // auto tokens = lexer.tokenize();
    std::cout << "词法分析完成" << std::endl;
#endif

#ifdef PARSER_ENABLED
    // TODO: 语法分析
    std::cout << "开始语法分析..." << std::endl;
    std::cout << "语法分析完成" << std::endl;
#endif

    std::cout << "编译完成" << std::endl;
    return 0;
}
