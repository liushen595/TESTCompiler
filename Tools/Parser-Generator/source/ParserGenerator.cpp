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
            throw PaserGeneratorException("Input file path is empty");
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

        // 计算FIRST集和FOLLOW集
        std::cout << "Computing FIRST sets..." << std::endl;
        grammar.computeFirstSets();

        std::cout << "Computing FOLLOW sets..." << std::endl;
        grammar.computeFollowSets();
    }

    void ParserGenerator::generateTable() {
        std::cout << "Generating LL(1) parsing table..." << std::endl;

        table = LL1Table(&grammar);
        table.build();

        tableBuilt = true;
    }

    void ParserGenerator::exportTable(const std::string& filename) {
        if (!tableBuilt) {
            throw PaserGeneratorException("Parsing table not generated yet");
        }

        std::cout << "Exporting parsing table to: " << filename << std::endl;
        table.exportToHeaderFile(filename);
    }

    void ParserGenerator::printGrammarInfo() const {
        std::cout << "\n========================================" << std::endl;
        std::cout << "    Grammar Information" << std::endl;
        std::cout << "========================================" << std::endl;

        grammar.print();
        grammar.printFirstSets();
        grammar.printFollowSets();
    }

    void ParserGenerator::printTableInfo() const {
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

    void ParserGenerator::run() {
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

            // 5. 导出分析表
            if (!outputFile.empty()) {
                exportTable(outputFile);
            }

            // 6. 打印分析表信息
            // printTableInfo();

            std::cout << "\nGeneration complete!" << std::endl;
        }
        catch (const PaserGeneratorException& e) {
            throw e; // 重新抛出异常
        }
    }
} // namespace Compiler
