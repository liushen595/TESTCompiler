#include "ParserGenerator.hpp"
#include "Grammar.hpp"
#include <iostream>
#include <string>

using namespace Compiler;

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " <input grammar file> <output table file>" << std::endl;
    std::cout << "\nExample:" << std::endl;
    std::cout << "  " << programName << " syntax_rules_processed.txt parser_table.txt" << std::endl;
    std::cout << "\nNotes:" << std::endl;
    std::cout << "  - Input file: text file containing LL(1) grammar rules" << std::endl;
    std::cout << "  - Output file: generated LL(1) parsing table file" << std::endl;
    std::cout << "  - A corresponding .hpp header file will also be generated for compiler use" << std::endl;
}

int main(int argc, char* argv[]) {
    // 设置控制台输出为UTF-8（Windows系统）
#ifdef _WIN32
    system("chcp 65001 > nul");
#endif

    std::cout << "========================================" << std::endl;
    std::cout << "   LL(1) Parser Generator v1.0" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;

    // 检查命令行参数
    if (argc != 3) {
        printUsage(argv[0]);
        return 1;
    }

    std::string inputFile = argv[1];
    std::string outputFile = argv[2];

    std::cout << "Input file: " << inputFile << std::endl;
    std::cout << "Output file: " << outputFile << std::endl;
    std::cout << std::endl;

    try {
        // 创建生成器
        ParserGenerator generator(inputFile, outputFile);

        // 运行生成流程
        generator.run();

        std::cout << "\n========================================" << std::endl;
        std::cout << "   Generation successful!" << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << "Generated files:" << std::endl;
        std::cout << "  - " << outputFile << " (parsing table data)" << std::endl;
        std::cout << "  - " << outputFile << ".hpp (C++ header file)" << std::endl;

    }
    catch (const PaserGeneratorException& e) {
        std::cerr << "\n" << e.getFullMessage() << std::endl;
        return 1;
    }
    catch (const std::exception& e) {
        std::cerr << "\n\033[31mException: " << e.what() << "\033[0m" << std::endl;
        return 1;
    }
    catch (...) {
        std::cerr << "\n\033[31mAn unknown exception occurred!\033[0m" << std::endl;
        return 1;
    }

    return 0;
}
