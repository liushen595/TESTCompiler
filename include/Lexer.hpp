#pragma once

#ifndef LEXER_HPP
#define LEXER_HPP

#include <string>
#include <vector>
#include <memory>
#include <iostream>

namespace Compiler {

    // 令牌类型枚举
    enum class TokenType {
        // 基本令牌类型
        IDENTIFIER,     // 标识符
        NUMBER,         // 数字
        KEYWORD,        // 关键字
        COMPARISON_DOUBLE, // 双字符比较操作符
        COMPARISON_SINGLE, // 单字符比较操作符
        DIVISION,       // 除号
        COMMENT_FIRST,  // 注释开始
        COMMENT_LAST,   // 注释结束
        SINGLEWORD,    // 单字符符号
        EOF_TOKEN,      // 文件结束
        UNKNOWN         // 未知令牌
    };

    // 令牌结构
    struct Token {
        TokenType type; // 令牌类型
        std::string value; // 令牌值
        std::size_t line;   // 行号
        std::size_t column; // 列号
        std::size_t position; // 在输入中的位置

        Token(TokenType t, const std::string& v, std::size_t l, std::size_t c, std::size_t p)
            : type(t), value(v), line(l), column(c), position(p) {}
    };

    // 词法分析器类
    class Lexer {
    private:
        std::string input_;
        std::size_t position_;
        std::size_t line_;
        std::size_t column_;

        char currentChar();
        char peekChar(std::size_t offset = 1);
        void advance();
        void skipWhitespace();
        void skipComment();

        // DFA 驱动的词法分析
        Token runDFA();
        TokenType mapTokenName(const std::string& tokenName);

    public:
        Lexer(const std::string& input);

        // 获取下一个令牌
        Token nextToken();

        // 查看下一个令牌但不消费它
        Token peekToken();

        // 检查是否到达输入结尾
        bool isAtEnd() const;

        // 令牌化整个输入
        std::vector<Token> tokenize();

        // 重置词法分析器
        void reset();

        // 获取当前位置信息
        std::size_t getPosition() const { return position_; }
        std::size_t getLine() const { return line_; }
        std::size_t getColumn() const { return column_; }
    };

    // 令牌类型到字符串的转换
    std::string tokenTypeToString(TokenType type);

    // 检查是否为关键字
    bool isKeyword(const std::string& identifier);

    // 词法分析异常类
    class LexerException : public std::exception {
    private:
        std::string message_;
        std::size_t line_;
        std::size_t column_;

    public:
        LexerException(const std::string& message, std::size_t line, std::size_t column)
            : message_(message), line_(line), column_(column) {}

        const char* what() const noexcept override {
            return message_.c_str();
        }

        std::size_t getLine() const { return line_; }
        std::size_t getColumn() const { return column_; }

        std::string getFullMessage() const {
            return "LexError (in line:" + std::to_string(line_) +
                ", in column:" + std::to_string(column_) + "): " + message_;
        }
    };
} // namespace Compiler

// 输出词法分析结果，格式适合语法分析器使用
// 输出格式: <TokenType, TokenValue, Line, Column>
void outputLexerResults(const std::vector<Compiler::Token>& tokens, std::ostream& out = std::cout);

#endif // LEXER_HPP
