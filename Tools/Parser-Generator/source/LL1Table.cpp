#include "../header/LL1Table.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>

namespace Compiler {

    // ==================== TableEntry类实现 ====================

    TableEntry::TableEntry() : valid(false), productionIndex(-1), action("") {}

    TableEntry::TableEntry(int index, const std::string& action)
        : valid(true), productionIndex(index), action(action) {}

    // ==================== LL1Table类实现 ====================

    LL1Table::LL1Table() : grammar(nullptr) {}

    LL1Table::LL1Table(const Grammar* gra) : grammar(gra) {}

    void LL1Table::build() {
        // TODO: 构建LL(1)分析表
        // 算法步骤：
        // 1. 对每个产生式 A -> α：
        //    a. 对FIRST(α)中的每个终结符a，将A -> α填入table[A, a]
        //    b. 如果ε在FIRST(α)中，对FOLLOW(A)中的每个符号b，将A -> α填入table[A, b]
        // 2. 检测冲突

        if (!grammar) {
            throw PaserGeneratorException("\033[31mGrammar not initialized\033[0m");
        }

        try {
            for (const auto &prod : grammar->getProductions()) {
                Symbol A = prod.left;
                std::set<Symbol> firstalpha = grammar->getFirstSets().at(A);
                std::set<Symbol> followA = grammar->getFollowSets().at(A);
                for (Symbol a : firstalpha) {
                    if (!a.isEpsilon()) {
                        fillTableEntry(A, a, prod.index);
                    }
                    else {
                        for (Symbol b : followA) {
                            fillTableEntry(A, b, prod.index);
                        }
                    }
                }
            }
        }
        catch (const PaserGeneratorException& e) {
            throw e; // 重新抛出异常
        }


    }

    TableEntry LL1Table::query(const Symbol& nonTerminal, const Symbol& terminal) const {
        // TODO: 查询分析表
        // 返回table[nonTerminal, terminal]对应的表项

        auto key = std::make_pair(nonTerminal, terminal);
        auto it = table.find(key);
        if (it != table.end()) {
            return it->second;
        }
        return TableEntry();
    }

    void LL1Table::exportToHeaderFile(const std::string& filename) const {
        // TODO: 导出为C++头文件
        // 生成静态数据结构，供编译器主程序使用

        std::ofstream file(filename);
        if (!file.is_open()) {
            throw PaserGeneratorException("\033[31mFailed to create file: " + filename + "\033[0m");
        }

        // 写入头文件保护
        file << "#ifndef PARSER_TABLE_HPP\n";
        file << "#define PARSER_TABLE_HPP\n\n";
        file << "#include <string>\n";
        file << "#include <map>\n";
        file << "#include <vector>\n\n";

        // TODO: 生成数据结构
        // 1. 终结符和非终结符的枚举或映射
        // 2. 产生式数组
        // 3. 分析表数组或映射

        file << "#endif // PARSER_TABLE_HPP\n";
    }

    void LL1Table::print() const {
        // TODO: 打印分析表
        // 以表格形式输出，行为非终结符，列为终结符

        std::cout << "\n========== LL(1)分析表 ==========" << std::endl;

        if (!grammar) {
            throw PaserGeneratorException("\033[31mGrammar not initialized\033[0m");
        }

        // 实现细节待补充
    }

    void LL1Table::fillTableEntry(const Symbol& nonTerminal, const Symbol& terminal, int productionIndex) {
        auto key = std::make_pair(nonTerminal, terminal);
        if (table.find(key) != table.end()) {
            throw PaserGeneratorException("\033[31mConflict: table[" + nonTerminal.toString() +
                ", " + terminal.toString() + "] already has a production\033[0m");
        }
        table[key] = TableEntry(productionIndex);
    }

} // namespace ParserGenerator
