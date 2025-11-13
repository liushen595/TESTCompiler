#ifndef LL1TABLE_HPP
#define LL1TABLE_HPP

#include "Grammar.hpp"
#include <string>
#include <memory>
#include <map>
#include <utility>

namespace Compiler {

    // 分析表项
    struct TableEntry {
        bool valid;                     // 是否有效
        int productionIndex;            // 产生式索引
        std::string action;             // 动作描述

        TableEntry();
        TableEntry(int index, const std::string& action = "");
    };

    // LL(1)分析表类
    class LL1Table {
    private:
        const Grammar* grammar;

        // 分析表: [非终结符][终结符] -> 产生式索引
        std::map<std::pair<Symbol, Symbol>, TableEntry> table;

        // 同步符号集（用于错误恢复）
        std::map<Symbol, std::set<Symbol>> syncSets;

        // 填充分析表项
        void fillTableEntry(const Symbol& nonTerminal, const Symbol& terminal, int productionIndex);

    public:
        LL1Table();
        explicit LL1Table(const Grammar* gra);

        // 构建分析表
        void build();

        // 查询分析表
        TableEntry query(const Symbol& nonTerminal, const Symbol& terminal) const;

        // 导出分析表到头文件
        void exportToHeaderFile(const std::string& filename) const;

        // 打印分析表
        void print() const;
    };

} // namespace Compiler

#endif // LL1TABLE_HPP
