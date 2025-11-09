#include "../header/Grammar.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>

namespace Compiler {

    // ==================== Symbol类实现 ====================

    Symbol::Symbol() : name(""), type(SymbolType::TERMINAL) {}

    Symbol::Symbol(const std::string& name, SymbolType type)
        : name(name), type(type) {}

    bool Symbol::operator<(const Symbol& other) const {
        if (type != other.type) {
            return type < other.type;
        }
        return name < other.name;
    }

    bool Symbol::operator==(const Symbol& other) const {
        return name == other.name && type == other.type;
    }

    bool Symbol::isTerminal() const {
        return type == SymbolType::TERMINAL;
    }

    bool Symbol::isNonTerminal() const {
        return type == SymbolType::NON_TERMINAL;
    }

    bool Symbol::isEpsilon() const {
        return type == SymbolType::EPSILON;
    }

    std::string Symbol::toString() const {
        switch (type) {
        case SymbolType::TERMINAL:
            return "\"" + name + "\"";
        case SymbolType::NON_TERMINAL:
            return "<" + name + ">";
        case SymbolType::EPSILON:
            return "ε";
        default:
            return name;
        }
    }

    // ==================== Production类实现 ====================

    Production::Production() {}

    Production::Production(const Symbol& left, const std::vector<Symbol>& right, int idx)
        : left(left), right(right), index(idx) {}

    std::string Production::toString() const {
        std::string result = left.toString() + " -> ";
        if (right.empty() || (right.size() == 1 && right[0].isEpsilon())) {
            result += "ε";
        }
        else {
            for (size_t i = 0; i < right.size(); ++i) {
                if (i > 0) result += " ";
                result += right[i].toString();
            }
        }
        return result;
    }

    bool Production::hasEpsilon() const {
        return right.empty() || (right.size() == 1 && right[0].isEpsilon());
    }

    // ==================== Grammar类实现 ====================

    Grammar::Grammar() {}

    void Grammar::loadFromFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw PaserGeneratorException("\033[31mFailed to open file: " + filename + "\033[0m");
        }

        std::string line;
        int index = 0;
        while (std::getline(file, line)) {
            parseGrammarLine(line, index++);
        }

        if (productions.empty()) {
            throw PaserGeneratorException("\033[31mNo productions found in grammar file\033[0m");
        }

        startSymbol = productions[0].left; // Assume the left-hand side of the first production is the start symbol
    }

    void Grammar::parseGrammarLine(const std::string& line, int index) {
        Production production;
        std::vector<Symbol> rightSide;
        std::stringstream ss(line);
        std::string left, right;
        if (!(ss >> left)) {
            throw PaserGeneratorException("\033[31mEncountered error while parsing grammar line.\033[0m");
        }

        // 解析左部非终结符
        Symbol leftSymbol(left, SymbolType::NON_TERMINAL);
        nonTerminals.insert(leftSymbol);

        try {
            while (ss >> right) {
                if (right[0] == '<' && right.back() == '>') {
                    // 非终结符
                    Symbol rightSymbol(right, SymbolType::NON_TERMINAL);
                    rightSide.push_back(rightSymbol);
                    nonTerminals.insert(rightSymbol);
                }
                else if (right[0] == '|') {
                    // 分隔符，保存当前产生式，开始新产生式
                    productions.push_back(Production(leftSymbol, rightSide, index++));
                    rightSide.clear();
                }
                else if (right == "ε") {
                    // ε符号
                    Symbol rightSymbol("ε", SymbolType::EPSILON);
                    rightSide.push_back(rightSymbol);
                }
                else {
                    // 终结符
                    rightSide.push_back(Symbol(right, SymbolType::TERMINAL));
                    terminals.insert(Symbol(right, SymbolType::TERMINAL));
                }
            }
            // 保存最后一个产生式
            if (!rightSide.empty()) {
                productions.push_back(Production(leftSymbol, rightSide, index++));
            }
        }
        catch (...) {
            throw PaserGeneratorException("\033[31mEncountered error while parsing grammar line.\033[0m");
        }
    }

    void Grammar::computeFirstSets() {
        // TODO: 计算所有非终结符的FIRST集
        // 算法步骤：
        // 1. 初始化所有FIRST集为空
        // 2. 对每个产生式 A -> α：
        //    - 将FIRST(α)加入FIRST(A)
        // 3. 重复直到不再变化

        for (const auto &nt : nonTerminals) {
            firstSets[nt] = std::set<Symbol>();
        }

        bool changed = true;
        while (changed) {
            changed = false;
            for (const auto &prod : productions) {
                Symbol A = prod.left;
                std::set<Symbol> firstalpha = computeFirst(prod.right);
                size_t beforeSize = firstSets[A].size();
                firstSets[A].insert(firstalpha.begin(), firstalpha.end());
                if (firstSets[A].size() > beforeSize) {
                    changed = true;
                }
            }
        }
    }

    std::set<Symbol> Grammar::computeFirst(const std::vector<Symbol>& symbols) const {
        // TODO: 计算符号串的FIRST集
        // 算法步骤：
        // 1. 如果第一个符号是终结符，返回{该终结符}
        // 2. 如果第一个符号是非终结符：
        //    - 将其FIRST集（除ε外）加入结果
        //    - 如果包含ε，继续处理下一个符号
        // 3. 如果所有符号都能推出ε，将ε加入结果

        std::set<Symbol> result;

        if (symbols.empty() || symbols[0].isEpsilon()) {
            result.insert(Symbol("ε", SymbolType::EPSILON));
            return result;
        }

        for (size_t i = 0; i < symbols.size(); i++) {
            if (symbols[i].isTerminal()) {
                result.insert(symbols[i]);
                break;
            }

            // 非终结符, 添加其 FIRST 集（除 ε）
            auto it = firstSets.find(symbols[i]);
            if (it == firstSets.end()) {
                // 如果找不到该非终结符的 FIRST 集，跳过
                break;
            }
            const auto& firstSet = it->second;
            for (const auto &sym : firstSet) {
                if (!sym.isEpsilon()) {
                    result.insert(sym);
                }
            }

            // 如果 FIRST(symbols[i]) 不包含 ε，停止
            if (firstSet.find(Symbol("ε", SymbolType::EPSILON)) == firstSet.end()) {
                break;
            }

            // 如果到达最后一个符号且都包含 ε，加入 ε
            if (i == symbols.size() - 1) {
                result.insert(Symbol("ε", SymbolType::EPSILON));
            }
        }

        return result;
    }

    void Grammar::computeFollowSets() {
        // TODO: 计算所有非终结符的FOLLOW集
        // 算法步骤：
        // 1. 将$加入开始符号的FOLLOW集
        // 2. 对每个产生式 A -> αBβ：
        //    - 将FIRST(β)（除ε外）加入FOLLOW(B)
        //    - 如果β能推出ε，将FOLLOW(A)加入FOLLOW(B)
        // 3. 重复直到不再变化
        for (const auto &nt : nonTerminals) {
            followSets[nt] = std::set<Symbol>();
        }
        followSets[startSymbol].insert(Symbol("$", SymbolType::TERMINAL));

        bool changed = true;
        while (changed) {
            changed = false;

            for (const auto &prod : productions) {
                const auto &A = prod.left;
                const std::vector<Symbol> &right = prod.right;

                for (size_t i = 0; i < right.size(); i++) {
                    if (right[i].isNonTerminal()) {
                        size_t beforeSize = followSets[right[i]].size();

                        // 计算β的FIRST集
                        std::vector<Symbol> beta(right.begin() + i + 1, right.end());
                        std::set<Symbol> beta_first = computeFirst(beta);

                        // 添加 FIRST(β) - {ε}
                        for (const auto &sym : beta_first) {
                            if (!sym.isEpsilon()) {
                                followSets[right[i]].insert(sym);
                            }
                        }

                        // 如果 β能推出 ε，添加 FOLLOW(A)
                        if (beta_first.find(Symbol("ε", SymbolType::EPSILON)) != beta_first.end() || beta.empty()) {
                            const auto &followA = followSets[A];
                            followSets[right[i]].insert(followA.begin(), followA.end());
                        }

                        if (followSets[right[i]].size() > beforeSize) {
                            changed = true;
                        }
                    }
                }
            }
        }
    }

    // ==================== Getter方法 ====================

    const Symbol& Grammar::getStartSymbol() const {
        return startSymbol;
    }

    const std::set<Symbol>& Grammar::getTerminals() const {
        return terminals;
    }

    const std::set<Symbol>& Grammar::getNonTerminals() const {
        return nonTerminals;
    }

    const std::vector<Production>& Grammar::getProductions() const {
        return productions;
    }

    const std::map<Symbol, std::set<Symbol>>& Grammar::getFirstSets() const {
        return firstSets;
    }

    const std::map<Symbol, std::set<Symbol>>& Grammar::getFollowSets() const {
        return followSets;
    }

    // ==================== 工具方法 ====================

    void Grammar::print() const {
        std::cout << "========== Grammar Information ==========" << std::endl;
        std::cout << "Start Symbol: " << startSymbol.toString() << std::endl;

        std::cout << "\nNon-Terminals: ";
        for (const auto& nt : nonTerminals) {
            std::cout << nt.toString() << " ";
        }
        std::cout << std::endl;

        std::cout << "\nTerminals: ";
        for (const auto& t : terminals) {
            std::cout << t.toString() << " ";
        }
        std::cout << std::endl;

        std::cout << "\nProductions:" << std::endl;
        for (size_t i = 0; i < productions.size(); ++i) {
            std::cout << i << ": " << productions[i].toString() << std::endl;
        }
    }

    void Grammar::printFirstSets() const {
        std::cout << "\n========== FIRST Sets ==========" << std::endl;
        for (const auto& pair : firstSets) {
            std::cout << "FIRST(" << pair.first.toString() << ") = { ";
            for (const auto& sym : pair.second) {
                std::cout << sym.toString() << " ";
            }
            std::cout << "}" << std::endl;
        }
    }

    void Grammar::printFollowSets() const {
        std::cout << "\n========== FOLLOW Sets ==========" << std::endl;
        for (const auto& pair : followSets) {
            std::cout << "FOLLOW(" << pair.first.toString() << ") = { ";
            for (const auto& sym : pair.second) {
                std::cout << sym.toString() << " ";
            }
            std::cout << "}" << std::endl;
        }
    }

} // namespace Compiler
