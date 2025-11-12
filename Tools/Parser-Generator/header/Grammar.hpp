#ifndef GRAMMAR_HPP
#define GRAMMAR_HPP

#include <string>
#include <vector>
#include <set>
#include <map>
#include <iostream>

namespace Compiler {

    // 符号类型枚举
    enum class SymbolType {
        TERMINAL,       // 终结符
        NON_TERMINAL,   // 非终结符
        EPSILON         // 空符号 ε
    };

    // 符号类
    class Symbol {
    public:
        std::string name;
        SymbolType type;

        Symbol();
        Symbol(const std::string& name, SymbolType type);

        // 符号比较<
        bool operator<(const Symbol& other) const;
        // 符号比较==
        bool operator==(const Symbol& other) const;
        // 是否是终结符
        bool isTerminal() const;
        // 是否是非终结符
        bool isNonTerminal() const;
        // 是否是ε
        bool isEpsilon() const;

        // 转为字符串表示
        std::string toString() const;
    };

    // 产生式类
    class Production {
    public:
        Symbol left;                    // 左部（非终结符）
        std::vector<Symbol> right;      // 右部（符号串）
        int index;

        Production();
        Production(const Symbol& left, const std::vector<Symbol>& right, int idx);

        // 转为字符串表示
        std::string toString() const;
        // 是否包含ε
        bool hasEpsilon() const;
    };

    // 文法类
    class Grammar {
    private:
        Symbol startSymbol;                                     // 开始符号
        std::set<Symbol> terminals;                             // 终结符集合
        std::set<Symbol> nonTerminals;                          // 非终结符集合
        std::vector<Production> productions;                    // 产生式列表

        std::map<Symbol, std::set<Symbol>> firstSets;          // FIRST集
        std::map<Symbol, std::set<Symbol>> followSets;         // FOLLOW集

    public:
        Grammar();

        // 语法规则加载
        void loadFromFile(const std::string& filename);

        // 语法规则解析
        void parseGrammarLine(const std::string& line);

        // 计算FIRST集
        void computeFirstSets();
        std::set<Symbol> computeFirst(const std::vector<Symbol>& symbols) const;

        // 计算FOLLOW集
        void computeFollowSets();

        // 获取开始符号
        const Symbol& getStartSymbol() const;
        // 获取终结符集合
        const std::set<Symbol>& getTerminals() const;
        // 获取非终结符集合
        const std::set<Symbol>& getNonTerminals() const;
        // 获取产生式列表
        const std::vector<Production>& getProductions() const;
        // 获取FIRST集
        const std::map<Symbol, std::set<Symbol>>& getFirstSets() const;
        // 获取FOLLOW集
        const std::map<Symbol, std::set<Symbol>>& getFollowSets() const;

        // 工具方法
        void print() const;
        void printFirstSets() const;
        void printFollowSets() const;
    };

    class PaserGeneratorException : public std::exception {
    private:
        std::string message_;
    public:
        PaserGeneratorException(const std::string& message)
            : message_(message) {}

        const char* what() const noexcept override {
            return message_.c_str();
        }

        std::string getFullMessage() const {
            return "Error: " + message_;
        }
    };

} // namespace Compiler

#endif // GRAMMAR_HPP
