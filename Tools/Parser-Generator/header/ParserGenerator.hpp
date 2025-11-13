#ifndef PARSER_GENERATOR_HPP
#define PARSER_GENERATOR_HPP

#include "Grammar.hpp"
#include "LL1Table.hpp"
#include <string>

namespace Compiler {

    // 语法分析器生成器主类
    class ParserGenerator {
    private:
        Grammar grammar;
        LL1Table table;

        std::string inputFile;          // 输入语法规则文件
        std::string outputFile;         // 输出分析表文件

        bool initialized;
        bool tableBuilt;
    public:
        ParserGenerator();
        ParserGenerator(const std::string& inputFile, const std::string& outputFile);

        // 初始化
        void initialize();

        // 加载语法规则
        void loadGrammar(const std::string& filename);

        // 生成分析表
        void generateTable();

        // 导出分析表
        void exportTable(const std::string& filename);

        // 运行完整流程
        void run();

        // 打印信息
        void printGrammarInfo() const;
        void printTableInfo() const;

        // Getter方法
        const Grammar& getGrammar() const;
        const LL1Table& getTable() const;
    };

} // namespace Compiler

#endif // PARSER_GENERATOR_HPP
