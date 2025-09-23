#pragma once

#include <string>
#include <vector>
#include <memory>

namespace Compiler {

    // 令牌类型枚举
    enum class TokenType {
        // 基本令牌类型
        IDENTIFIER,     // 标识符
        NUMBER,         // 数字
        STRING,         // 字符串字面量
        KEYWORD,        // 关键字
        OPERATOR,       // 操作符
        DELIMITER,      // 分隔符
        WHITESPACE,     // 空白字符
        COMMENT,        // 注释
        NEWLINE,        // 换行
        EOF_TOKEN,      // 文件结束
        UNKNOWN         // 未知令牌
    };

    // 令牌结构
    struct Token {
        TokenType type;
        std::string value;
        std::size_t line;
        std::size_t column;
        std::size_t position;

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

        Token readIdentifier();
        Token readNumber();
        Token readString();
        Token readOperator();

    public:
        explicit Lexer(const std::string& input);

        // 获取下一个令牌
        Token nextToken();

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

} // namespace Compiler
