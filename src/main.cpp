#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <map>
#include <memory>

// 引入分析器头文件
#include "Lexer.hpp"
#include "Parser.hpp"

int main(int argc, char* argv[]) {
    std::cout << "TESTCompiler - Compiler" << std::endl;

    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <input_file>" << std::endl;
        return 1;
    }

    std::string inputFile = argv[1];
    std::cout << "Processing file: " << inputFile << std::endl;

    // 检查文件是否存在
    std::ifstream file(inputFile);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file " << inputFile << std::endl;
        return 1;
    }

    // 读取文件内容
    std::string content;
    std::string line;
    while (std::getline(file, line)) {
        content += line + "\n";
    }
    file.close();

    std::cout << "File content loaded successfully, size: " << content.size() << " bytes" << std::endl;

    try {
        std::cout << "\n" << std::string(60, '=') << std::endl;
        std::cout << "Starting compilation analysis..." << std::endl;
        std::cout << std::string(60, '=') << std::endl;
        // 主流程：语法分析驱动编译过程
        std::cout << "Creating lexer..." << std::endl;
        auto lexer = std::make_shared<Compiler::Lexer>(content);

        std::cout << "Creating parser..." << std::endl;
        Compiler::Parser parser(lexer);

        std::cout << "Starting syntax analysis..." << std::endl;
        parser.parse();

        std::cout << "\n" << std::string(60, '=') << std::endl;
        std::cout << "Printing Abstract Syntax Tree..." << std::endl;
        std::cout << std::string(60, '=') << std::endl;
        parser.printAST(std::cerr);
    }
    catch (const Compiler::LexerException &ex) {
        std::cerr << "\033[31m" << ex.getFullMessage() << "\033[0m" << std::endl;
        std::cerr << "Compilation terminated due to lexical errors." << std::endl;
        return 1;
    }
    catch (const Compiler::ParseException &ex) {
        std::cerr << "\033[31m" << ex.getFullMessage() << "\033[0m" << std::endl;
        std::cerr << "Compilation terminated due to syntax errors." << std::endl;
        return 1;
    }
    catch (const std::exception &ex) {
        std::cerr << "\033[31mUnexpected error: " << ex.what() << "\033[0m" << std::endl;
        std::cerr << "Compilation terminated due to unexpected errors." << std::endl;
        return 1;
    }

    std::cout << "Compilation completed!" << std::endl;
    return 0;
}
