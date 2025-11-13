#pragma once

#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <memory>
#include <stack>
#include "Lexer.hpp"
#include "LL1_Table.hpp"
#include "AST.hpp"

namespace Compiler {

    // 语法分析错误类
    class ParseException : public std::exception {
    private:
        std::string message_;
        std::size_t line_;
        std::size_t column_;

    public:
        ParseException(const std::string& message, std::size_t line, std::size_t column)
            : message_(message), line_(line), column_(column) {}

        const char* what() const noexcept override {
            return message_.c_str();
        }

        std::size_t getLine() const { return line_; }
        std::size_t getColumn() const { return column_; }

        std::string getFullMessage() const {
            return "SyntaxError (in line:" + std::to_string(line_) +
                ", in column:" + std::to_string(column_) + "): " + message_;
        }
    };

    // 语法分析器类
    class Parser {
    private:
        std::shared_ptr<Lexer> lexer_; // 词法分析器智能指针
        Token currentToken_; // 当前token
        std::stack<std::pair<std::string, SymbolType>> parseStack; // 分析栈
        std::stack<std::shared_ptr<ASTNode>> astStack; // AST构造栈
        std::stack<int> productionStack; // 产生式栈，记录待规约的产生式索引
        std::shared_ptr<ASTNode> astRoot_; // AST根节点

        // 获取下一个token
        void advance();

        // 将Token转换为终结符字符串
        std::string tokenToTerminal(const Token& token);

        // 初始化分析栈
        void initializeStack();

        // 获取产生式索引
        int getProductionIndex(const std::string& nonTerminal, const std::string& terminal);

        // 获取当前token位置信息
        std::size_t getCurrentLine() const;
        std::size_t getCurrentColumn() const;

        // AST构造相关方法
        void buildASTNode(const Production& prod);

    public:
        // 构造函数 - 接受词法分析器智能指针
        Parser(std::shared_ptr<Lexer> lexer);

        // 构造函数 - 从输入字符串创建
        Parser(const std::string& input);

        // 执行语法分析
        void parse();

        // 获取AST根节点
        std::shared_ptr<ASTNode> getAST() const { return astRoot_; }

        // 打印AST
        void printAST(std::ostream& os = std::cout) const;
    };

} // namespace Compiler

#endif // PARSER_HPP