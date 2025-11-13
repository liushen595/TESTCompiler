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
        if (!grammar) {
            throw PaserGeneratorException("Grammar not initialized");
        }

        try {
            for (const auto &prod : grammar->getProductions()) {
                Symbol A = prod.left;

                // 计算产生式右部的 FIRST 集
                std::set<Symbol> firstalpha = grammar->computeFirst(prod.right);
                std::set<Symbol> followA = grammar->getFollowSets().at(A);

                bool hasEpsilon = false;
                for (const Symbol& a : firstalpha) {
                    if (!a.isEpsilon()) {
                        fillTableEntry(A, a, prod.index);
                    }
                    else {
                        hasEpsilon = true;
                    }
                }

                // 如果 FIRST(α) 包含 ε，则将产生式加入 FOLLOW(A) 的每个符号
                if (hasEpsilon) {
                    for (const Symbol& b : followA) {
                        fillTableEntry(A, b, prod.index);
                    }
                }
            }
        }
        catch (const PaserGeneratorException& e) {
            throw e;
        }
    }

    TableEntry LL1Table::query(const Symbol& nonTerminal, const Symbol& terminal) const {
        auto key = std::make_pair(nonTerminal, terminal);
        auto it = table.find(key);
        if (it != table.end()) {
            return it->second;
        }
        return TableEntry();
    }

    void LL1Table::exportToHeaderFile(const std::string& filename) const {
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw PaserGeneratorException("Failed to create file: " + filename);
        }

        // 写入头文件保护
        file << "#ifndef PARSER_TABLE_HPP\n";
        file << "#define PARSER_TABLE_HPP\n\n";
        file << "#include <string>\n";
        file << "#include <map>\n";
        file << "#include <vector>\n\n";

        file << "namespace Compiler {\n\n";

        // 1. 导出符号类型枚举
        file << "// 符号类型\n";
        file << "enum class SymbolType {\n";
        file << "    TERMINAL,\n";
        file << "    NON_TERMINAL,\n";
        file << "    EPSILON\n";
        file << "};\n\n";

        // 2. 导出终结符映射
        file << "// 终结符映射\n";
        file << "const std::map<std::string, int> TERMINALS = {\n";
        int termIdx = 0;
        for (const auto& term : grammar->getTerminals()) {
            file << "    {\"" << term.name << "\", " << termIdx++ << "},\n";
        }
        file << "};\n\n";

        // 3. 导出非终结符映射
        file << "// 非终结符映射\n";
        file << "const std::map<std::string, int> NON_TERMINALS = {\n";
        int nonTermIdx = 0;
        for (const auto& nonTerm : grammar->getNonTerminals()) {
            file << "    {\"" << nonTerm.name << "\", " << nonTermIdx++ << "},\n";
        }
        file << "};\n\n";

        // 4. 导出产生式数组
        file << "// 产生式定义\n";
        file << "struct Production {\n";
        file << "    std::string left;\n";
        file << "    std::vector<std::pair<std::string, SymbolType>> right;\n";
        file << "};\n\n";

        file << "const std::vector<Production> PRODUCTIONS = {\n";
        for (const auto& prod : grammar->getProductions()) {
            file << "    {\"" << prod.left.name << "\", {\n";
            for (const auto& sym : prod.right) {
                file << "        {\"" << sym.name << "\", SymbolType::";
                if (sym.isTerminal()) file << "TERMINAL";
                else if (sym.isNonTerminal()) file << "NON_TERMINAL";
                else file << "EPSILON";
                file << "},\n";
            }
            file << "    }},\n";
        }
        file << "};\n\n";

        // 5. 导出分析表
        file << "// LL(1)分析表 [非终结符索引][终结符索引] = 产生式索引(-1表示空)\n";
        file << "const std::map<std::pair<int, int>, int> PARSING_TABLE = {\n";
        for (const auto& entry : table) {
            const auto& key = entry.first;
            const auto& value = entry.second;
            if (value.valid) {
                // 查找非终结符和终结符的索引
                int ntIdx = 0, tIdx = 0;
                for (const auto& nt : grammar->getNonTerminals()) {
                    if (nt == key.first) break;
                    ntIdx++;
                }
                for (const auto& t : grammar->getTerminals()) {
                    if (t == key.second) break;
                    tIdx++;
                }
                file << "    {{" << ntIdx << ", " << tIdx << "}, " << value.productionIndex << "},\n";
            }
        }
        file << "};\n\n";

        file << "} // namespace Compiler\n\n";
        file << "#endif // PARSER_TABLE_HPP\n";

        file.close();
        std::cout << "Successfully exported parsing table to: " << filename << std::endl;
    }

    void LL1Table::print() const {
        std::cout << "\n========== LL(1)分析表 ==========" << std::endl;

        if (!grammar) {
            throw PaserGeneratorException("Grammar not initialized");
        }

        // 获取所有终结符和非终结符
        const auto& terminals = grammar->getTerminals();
        const auto& nonTerminals = grammar->getNonTerminals();
        const auto& productions = grammar->getProductions();

        if (terminals.empty() || nonTerminals.empty()) {
            std::cout << "Empty grammar table" << std::endl;
            return;
        }

        // 计算列宽
        int maxWidth = 15;

        // 打印表头（终结符）
        std::cout << std::setw(maxWidth) << std::left << "";
        for (const auto& term : terminals) {
            std::cout << std::setw(maxWidth) << std::left << term.toString();
        }
        std::cout << std::endl;

        // 打印分隔线
        std::cout << std::string(maxWidth * (terminals.size() + 1), '-') << std::endl;

        // 打印每一行（每个非终结符）
        for (const auto& nonTerm : nonTerminals) {
            std::cout << std::setw(maxWidth) << std::left << nonTerm.toString();

            for (const auto& term : terminals) {
                TableEntry entry = query(nonTerm, term);
                if (entry.valid) {
                    std::string cellContent = std::to_string(entry.productionIndex);
                    std::cout << std::setw(maxWidth) << std::left << cellContent;
                }
                else {
                    std::cout << std::setw(maxWidth) << std::left << "-";
                }
            }
            std::cout << std::endl;
        }

        std::cout << "\n产生式列表：" << std::endl;
        for (size_t i = 0; i < productions.size(); ++i) {
            std::cout << i << ": " << productions[i].toString() << std::endl;
        }
    }

    void LL1Table::fillTableEntry(const Symbol& nonTerminal, const Symbol& terminal, int productionIndex) {
        auto key = std::make_pair(nonTerminal, terminal);

        if (table.find(key) != table.end()) {
            // 已存在表项，检测冲突
            const auto& existingEntry = table[key];
            const auto& productions = grammar->getProductions();

            // 获取当前产生式和已存在的产生式
            const Production* currentProd = nullptr;
            const Production* existingProd = nullptr;

            if (productionIndex >= 0 && productionIndex < static_cast<int>(productions.size())) {
                currentProd = &productions[productionIndex];
            }

            if (existingEntry.productionIndex >= 0 &&
                existingEntry.productionIndex < static_cast<int>(productions.size())) {
                existingProd = &productions[existingEntry.productionIndex];
            }

            // 检查是否是 ε 产生式
            bool currentIsEpsilon = currentProd && currentProd->hasEpsilon();
            bool existingIsEpsilon = existingProd && existingProd->hasEpsilon();

            // 冲突解决策略：优先选择非 ε 产生式（shift 优先于 reduce）
            if (currentIsEpsilon && !existingIsEpsilon) {
                // 当前是 ε，已存在的不是 ε，保留已存在的（shift）
                std::cerr << "\033[33mWarning: Conflict detected at table["
                    << nonTerminal.toString() << ", " << terminal.toString() << "]" << std::endl;
                if (existingProd) std::cerr << "  Existing: " << existingProd->toString() << std::endl;
                if (currentProd) std::cerr << "  New:      " << currentProd->toString() << std::endl;
                std::cerr << "  Resolution: Keeping non-epsilon production (shift over reduce)\033[0m" << std::endl;
                return; // 保留已存在的
            }
            else if (!currentIsEpsilon && existingIsEpsilon) {
                // 当前不是 ε，已存在的是 ε，用当前的替换（shift）
                std::cerr << "\033[33mWarning: Conflict detected at table["
                    << nonTerminal.toString() << ", " << terminal.toString() << "]" << std::endl;
                if (existingProd) std::cerr << "  Existing: " << existingProd->toString() << std::endl;
                if (currentProd) std::cerr << "  New:      " << currentProd->toString() << std::endl;
                std::cerr << "  Resolution: Using non-epsilon production (shift over reduce)\033[0m" << std::endl;
                table[key] = TableEntry(productionIndex);
                return;
            }
            else {
                // 两个都是 ε 或两个都不是 ε，这是真正的冲突，报错
                std::string msg = "Conflict: table[" + nonTerminal.toString() +
                    ", " + terminal.toString() + "] already has a production\n";
                if (existingProd) {
                    msg += "  Existing: " + existingProd->toString() + "\n";
                }
                if (currentProd) {
                    msg += "  New:      " + currentProd->toString() + "\n";
                }
                throw PaserGeneratorException(msg);
            }
        }

        // 没有冲突，直接填入
        table[key] = TableEntry(productionIndex);
    }

} // namespace ParserGenerator
