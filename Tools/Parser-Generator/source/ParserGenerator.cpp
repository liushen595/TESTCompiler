#include "../header/ParserGenerator.hpp"
#include <iostream>

namespace Compiler {

    // ==================== ParserGenerator类实现 ====================

    ParserGenerator::ParserGenerator()
        : initialized(false), tableBuilt(false) {}

    ParserGenerator::ParserGenerator(const std::string& inputFile, const std::string& outputFile)
        : inputFile(inputFile), outputFile(outputFile), initialized(false), tableBuilt(false) {}

    void ParserGenerator::initialize() {
        std::cout << "Initializing parser generator..." << std::endl;
        if (inputFile.empty() || outputFile.empty()) {
            throw PaserGeneratorException("\033[31mInput file path is empty\033[0m");
        }
        initialized = true;
    }

    void ParserGenerator::loadGrammar(const std::string& filename) {
        std::cout << "Loading grammar rules: " << filename << std::endl;

        try {
            grammar.loadFromFile(filename);
        }
        catch (const PaserGeneratorException& e) {
            throw e; // 重新抛出异常
        }

        // 预处理
        // preprocessGrammar();

        // 计算FIRST集和FOLLOW集
        std::cout << "Computing FIRST sets..." << std::endl;
        grammar.computeFirstSets();

        std::cout << "Computing FOLLOW sets..." << std::endl;
        grammar.computeFollowSets();

        grammar.printFirstSets();
        grammar.printFollowSets();
    }

    void ParserGenerator::generateTable() {
        // TODO: 生成LL(1)分析表
        // 1. 验证LL(1)性质
        // 2. 构建分析表
        // 3. 检测冲突

        std::cout << "Generating LL(1) parsing table..." << std::endl;

        validateLL1();

        table = LL1Table(&grammar);
        table.build();

        tableBuilt = true;
    }

    void ParserGenerator::exportTable(const std::string& filename) {
        // TODO: 导出分析表

        if (!tableBuilt) {
            throw PaserGeneratorException("\033[31mParsing table not generated yet\033[0m");
        }

        std::cout << "Exporting parsing table to: " << filename << std::endl;
        table.exportToHeaderFile(filename);
    }

    void ParserGenerator::generateHeaderFile(const std::string& filename) {
        // TODO: 生成C++头文件

        if (!tableBuilt) {
            throw PaserGeneratorException("\033[31mParsing table not generated yet\033[0m");
        }

        std::cout << "Generating header file: " << filename << std::endl;
        table.exportToHeaderFile(filename);
    }

    void ParserGenerator::run() {
        // TODO: 运行完整流程

        std::cout << "========================================" << std::endl;
        std::cout << "    LL(1) Parser Generator" << std::endl;
        std::cout << "========================================" << std::endl;

        try {
            // 1. 初始化
            initialize();

            // 2. 加载语法规则
            loadGrammar(inputFile);

            // 3. 打印文法信息
            printGrammarInfo();

            // 4. 生成分析表
            generateTable();

            // 5. 打印分析表信息
            printTableInfo();

            // 6. 导出分析表
            if (!outputFile.empty()) {
                exportTable(outputFile);

                // 生成C++头文件
                std::string headerFile = outputFile + ".hpp";
                generateHeaderFile(headerFile);
            }
            std::cout << "\nGeneration complete!" << std::endl;
        }
        catch (const PaserGeneratorException& e) {
            throw e; // 重新抛出异常
        }


    }

    void ParserGenerator::printGrammarInfo() const {
        // TODO: 打印文法信息

        std::cout << "\n========================================" << std::endl;
        std::cout << "    Grammar Information" << std::endl;
        std::cout << "========================================" << std::endl;

        grammar.print();
        grammar.printFirstSets();
        grammar.printFollowSets();
    }

    void ParserGenerator::printTableInfo() const {
        // TODO: 打印分析表信息

        std::cout << "\n========================================" << std::endl;
        std::cout << "    Parsing Table Information" << std::endl;
        std::cout << "========================================" << std::endl;

        table.print();
    }

    const Grammar& ParserGenerator::getGrammar() const {
        return grammar;
    }

    const LL1Table& ParserGenerator::getTable() const {
        return table;
    }

    void ParserGenerator::preprocessGrammar() {
        // TODO: 预处理文法
        // 1. 展开可选项 [...]
        // 2. 展开重复项 {...}
        // 3. 其他转换

        std::cout << "Preprocessing grammar..." << std::endl;
    }

} // namespace Compiler
