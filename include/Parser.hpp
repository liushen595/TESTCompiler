#pragma once

#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include "Lexer.hpp"

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
        std::vector<Token> tokens_; // 令牌流
        std::size_t currentIndex_; // 当前token索引
        Token currentToken_; // 当前token
        std::string tokenFilePath_; // Token文件路径

        // 从token文件读取tokens
        bool loadTokensFromFile(const std::string& tokenFile);

        // 获取下一个token
        void advance();

        // 获取当前token的值
        // std::string getCurrentTokenValue() const;

        // 检查当前token是否匹配期望的值
        bool match(const std::string& expected);

        // 消费一个期望的token
        void consume(const std::string& expected);

        // 获取当前token位置信息
        std::size_t getCurrentLine() const;
        std::size_t getCurrentColumn() const;

        // 抛出语法分析异常
        void throwParseError(const std::string& message);

        // 语法分析函数 - 对应每条语法规则
        // <program> → <declaration_list> <statement_list>
        void program();
        // <declaration_list> → <declaration_list> <declaration_stat> | ε
        void declaration_list();
        // <declaration_stat> → int ID;
        void declaration_stat();
        // <statement_list> → <statement_list> <statement> | ε
        void statement_list();
        // <statement> → <if_stat> | <while_stat> | <for_stat> | <read_stat> | <write_stat> | <compound_stat> | <expression_stat>
        void statement();
        // <if_stat> → if(<expression>) <statement> [else <statement>]
        void if_stat();
        // <while_stat> → while(<expression>) <statement>
        void while_stat();
        // <for_stat> → for(<expression>;<expression>;<expression>) <statement>
        void for_stat();
        // <read_stat> → read ID;
        void read_stat();
        // <write_stat> → write <expression>;
        void write_stat();
        // <compound_stat> → { <statement_list> }
        void compound_stat();
        // <expression_stat> → <expression>; | ;
        void expression_stat();
        // <expression> → ID=<bool_expr> | <bool_expr>
        void expression();
        // <bool_expr> → <additive_expr> | <additive_expr>(>|<|>=|<=|==|!=)<additive_expr>
        void bool_expr();
        // <additive_expr> → <term>{(+|-)<term>}
        void additive_expr();
        // <term> → <factor>{(*|/)<factor>}
        void term();
        // <factor> → (<expression>) | ID | NUM
        void factor();

    public:
        // 构造函数 - 接受token文件路径
        Parser(const std::string& tokenFile);

        // 构造函数 - 接受token向量
        Parser(const std::vector<Token>& tokens);

        // 执行语法分析
        void parse();
    };

} // namespace Compiler

#endif // PARSER_HPP