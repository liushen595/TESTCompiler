#include "Lexer.hpp"
#include <cctype>
#include <unordered_set>

namespace Compiler {

    // 关键字集合
    static const std::unordered_set<std::string> keywords = {
        "if", "else", "while", "for", "return", "int", "float", "char", "string",
        "bool", "true", "false", "void", "const", "struct", "class"
    };

    Lexer::Lexer(const std::string& input)
        : input_(input), position_(0), line_(1), column_(1) {}

    char Lexer::currentChar() {
        if (position_ >= input_.size()) {
            return '\0';  // EOF
        }
        return input_[position_];
    }

    char Lexer::peekChar(std::size_t offset) {
        std::size_t pos = position_ + offset;
        if (pos >= input_.size()) {
            return '\0';  // EOF
        }
        return input_[pos];
    }

    void Lexer::advance() {
        if (position_ < input_.size()) {
            if (currentChar() == '\n') {
                line_++;
                column_ = 1;
            }
            else {
                column_++;
            }
            position_++;
        }
    }

    void Lexer::skipWhitespace() {
        while (std::isspace(currentChar()) && currentChar() != '\n') {
            advance();
        }
    }

    void Lexer::skipComment() {
        // TODO: 实现注释跳过逻辑
        // 支持单行注释 //
        // 支持多行注释 /* */
    }

    Token Lexer::readIdentifier() {
        std::size_t startPos = position_;
        std::size_t startLine = line_;
        std::size_t startColumn = column_;
        std::string value;

        while (std::isalnum(currentChar()) || currentChar() == '_') {
            value += currentChar();
            advance();
        }

        TokenType type = isKeyword(value) ? TokenType::KEYWORD : TokenType::IDENTIFIER;
        return Token(type, value, startLine, startColumn, startPos);
    }

    Token Lexer::readNumber() {
        std::size_t startPos = position_;
        std::size_t startLine = line_;
        std::size_t startColumn = column_;
        std::string value;

        while (std::isdigit(currentChar()) || currentChar() == '.') {
            value += currentChar();
            advance();
        }

        return Token(TokenType::NUMBER, value, startLine, startColumn, startPos);
    }

    Token Lexer::readString() {
        std::size_t startPos = position_;
        std::size_t startLine = line_;
        std::size_t startColumn = column_;
        std::string value;

        char quote = currentChar();  // ' 或 "
        advance(); // 跳过开始引号

        while (currentChar() != quote && currentChar() != '\0') {
            value += currentChar();
            advance();
        }

        if (currentChar() == quote) {
            advance(); // 跳过结束引号
        }

        return Token(TokenType::STRING, value, startLine, startColumn, startPos);
    }

    Token Lexer::readOperator() {
        std::size_t startPos = position_;
        std::size_t startLine = line_;
        std::size_t startColumn = column_;
        std::string value;

        value += currentChar();
        advance();

        return Token(TokenType::OPERATOR, value, startLine, startColumn, startPos);
    }

    Token Lexer::nextToken() {
        skipWhitespace();

        char c = currentChar();

        if (c == '\0') {
            return Token(TokenType::EOF_TOKEN, "", line_, column_, position_);
        }

        if (c == '\n') {
            Token token(TokenType::NEWLINE, "\\n", line_, column_, position_);
            advance();
            return token;
        }

        if (std::isalpha(c) || c == '_') {
            return readIdentifier();
        }

        if (std::isdigit(c)) {
            return readNumber();
        }

        if (c == '"' || c == '\'') {
            return readString();
        }

        // 操作符和分隔符
        if (c == '+' || c == '-' || c == '*' || c == '/' || c == '=' ||
            c == '<' || c == '>' || c == '!' || c == '&' || c == '|') {
            return readOperator();
        }

        // 分隔符
        if (c == '(' || c == ')' || c == '{' || c == '}' || c == '[' || c == ']' ||
            c == ';' || c == ',' || c == '.') {
            Token token(TokenType::DELIMITER, std::string(1, c), line_, column_, position_);
            advance();
            return token;
        }

        // 未知字符
        Token token(TokenType::UNKNOWN, std::string(1, c), line_, column_, position_);
        advance();
        return token;
    }

    std::vector<Token> Lexer::tokenize() {
        std::vector<Token> tokens;

        while (true) {
            Token token = nextToken();
            tokens.push_back(token);

            if (token.type == TokenType::EOF_TOKEN) {
                break;
            }
        }

        return tokens;
    }

    void Lexer::reset() {
        position_ = 0;
        line_ = 1;
        column_ = 1;
    }

    std::string tokenTypeToString(TokenType type) {
        switch (type) {
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::NUMBER: return "NUMBER";
        case TokenType::STRING: return "STRING";
        case TokenType::KEYWORD: return "KEYWORD";
        case TokenType::OPERATOR: return "OPERATOR";
        case TokenType::DELIMITER: return "DELIMITER";
        case TokenType::WHITESPACE: return "WHITESPACE";
        case TokenType::COMMENT: return "COMMENT";
        case TokenType::NEWLINE: return "NEWLINE";
        case TokenType::EOF_TOKEN: return "EOF";
        case TokenType::UNKNOWN: return "UNKNOWN";
        default: return "UNKNOWN";
        }
    }

    bool isKeyword(const std::string& identifier) {
        return keywords.find(identifier) != keywords.end();
    }

} // namespace Compiler
