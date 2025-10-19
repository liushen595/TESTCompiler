#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <map>

// 引入词法分析器头文件
#ifdef LEXER_ENABLED
#include "Lexer.hpp"
#endif

#ifdef PARSER_ENABLED
#include "Parser.hpp"
#endif

int main(int argc, char* argv[]) {
    std::cout << "TESTCompiler - 编译器" << std::endl;

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
    // 生成输出文件路径（与输入文件同目录，添加.tokens后缀）
    std::string outputFile = inputFile + ".tokens";
    std::ofstream lexOut(outputFile);

    if (!lexOut.is_open()) {
        std::cerr << "错误: 无法创建输出文件 " << outputFile << std::endl;
        return 1;
    }

    try {
        // 词法分析
        std::cout << "\n" << std::string(60, '=') << std::endl;
        std::cout << "开始词法分析..." << std::endl;
        std::cout << std::string(60, '=') << std::endl;

        Compiler::Lexer lexer(content);
        std::vector<Compiler::Token> tokens = lexer.tokenize();

        std::cout << "\n词法分析完成！共识别 " << tokens.size() << " 个词法单元\n" << std::endl;

        // 输出词法分析结果到文件
        outputLexerResults(tokens, lexOut);
        lexOut.close();

        std::cout << "词法分析结果已保存到: " << outputFile << std::endl;
    }
    catch (const Compiler::LexerException &ex) {
        // 捕获词法分析异常，输出错误信息
        std::cerr << "\033[31m" << ex.getFullMessage() << "\033[0m" << std::endl;
        std::cerr << "Compilation terminated due to lex errors." << std::endl;
        // 清空输出文件内容
        outputLexerResults({}, lexOut);
        lexOut.close();
        return 1;
    }
#endif

#ifdef PARSER_ENABLED
    // TODO: 语法分析
    try {
        std::cout << "开始语法分析..." << std::endl;
        std::string tokenFile = inputFile + ".tokens";
        Compiler::Parser parser(tokenFile);
        parser.parse();
        std::cout << "语法分析完成" << std::endl;
    }
    catch (const Compiler::ParseException &ex) {
        std::cerr << "\033[31m" << ex.getFullMessage() << "\033[0m" << std::endl;
        std::cerr << "Compilation terminated due to parse errors." << std::endl;
        return 1;
    }
#endif

    std::cout << "编译完成" << std::endl;
    return 0;
}
