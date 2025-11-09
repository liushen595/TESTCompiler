#include "Parser.hpp"
#include <sstream>
#include <algorithm>

namespace Compiler {

    // 构造函数 - 接受词法分析器智能指针
    Parser::Parser(std::shared_ptr<Lexer> lexer)
        : lexer_(lexer), currentToken_(TokenType::EOF_TOKEN, "", 0, 0, 0) {
        if (lexer_ == nullptr) {
            throw ParseException("Lexer cannot be null", 0, 0);
        }
        // 获取第一个token
        advance();
    }

    // 构造函数 - 从输入字符串创建
    Parser::Parser(const std::string& input)
        : lexer_(std::make_shared<Lexer>(input)), currentToken_(TokenType::EOF_TOKEN, "", 0, 0, 0) {
        // 获取第一个token
        advance();
    }

    // 获取下一个token
    void Parser::advance() {
        if (lexer_ != nullptr && !lexer_->isAtEnd()) {
            try {
                currentToken_ = lexer_->nextToken();
            }
            catch (const LexerException& ex) {
                throw ParseException("Lexer error: " + std::string(ex.what()), ex.getLine(), ex.getColumn());
            }
        }
        else {
            // 到达输入结尾，设置为EOF token
            currentToken_ = Token(TokenType::EOF_TOKEN, "",
                lexer_ ? lexer_->getLine() : 0,
                lexer_ ? lexer_->getColumn() : 0,
                lexer_ ? lexer_->getPosition() : 0);
        }
    }

    // 检查当前token是否匹配期望的值
    bool Parser::match(const std::string& expected) {
        return currentToken_.value == expected;
    }

    // 检查当前token类型是否匹配期望的类型
    bool Parser::match(TokenType expectedType) {
        return currentToken_.type == expectedType;
    }

    // 消费一个期望的token
    void Parser::consume(const std::string& expected) {
        if (!match(expected)) {
            throwParseError("Expected '" + expected + "' but found '" + currentToken_.value + "'");
        }
        advance();
    }

    // 消费一个期望类型的token
    void Parser::consume(TokenType expectedType) {
        if (!match(expectedType)) {
            throwParseError("Expected " + tokenTypeToString(expectedType) +
                " but found " + tokenTypeToString(currentToken_.type) +
                " '" + currentToken_.value + "'");
        }
        advance();
    }

    // 获取当前token
    const Token& Parser::getCurrentToken() const {
        return currentToken_;
    }

    // 获取当前token位置信息
    std::size_t Parser::getCurrentLine() const {
        return currentToken_.line;
    }

    std::size_t Parser::getCurrentColumn() const {
        return currentToken_.column;
    }

    // 抛出语法分析异常
    void Parser::throwParseError(const std::string& message) {
        throw ParseException(message, getCurrentLine(), getCurrentColumn());
    }

    // 执行语法分析
    void Parser::parse() {
        // 注意：这是一个占位实现
        // 您将用LL分析器替换这个方法
        std::cout << "Parser::parse() - Ready for LL syntax analysis" << std::endl;
        std::cout << "Current token: " << tokenTypeToString(currentToken_.type)
            << " '" << currentToken_.value << "'" << std::endl;

        // 这里只是一个简单的演示，显示如何获取tokens
        while (currentToken_.type != TokenType::EOF_TOKEN) {
            std::cout << "Processing token: " << tokenTypeToString(currentToken_.type)
                << " '" << currentToken_.value << "' at line " << currentToken_.line << std::endl;
            advance();
        }

        std::cout << "Syntax analysis completed (placeholder implementation)" << std::endl;
    }

    // 以下方法是占位实现，您将用LL分析器替换它们
    void Parser::program() { /* Placeholder implementation */ }
    void Parser::declaration_list() { /* Placeholder implementation */ }
    void Parser::declaration_stat() { /* Placeholder implementation */ }
    void Parser::statement_list() { /* Placeholder implementation */ }
    void Parser::statement() { /* Placeholder implementation */ }
    void Parser::if_stat() { /* Placeholder implementation */ }
    void Parser::while_stat() { /* Placeholder implementation */ }
    void Parser::for_stat() { /* Placeholder implementation */ }
    void Parser::read_stat() { /* Placeholder implementation */ }
    void Parser::write_stat() { /* Placeholder implementation */ }
    void Parser::compound_stat() { /* Placeholder implementation */ }
    void Parser::expression_stat() { /* Placeholder implementation */ }
    void Parser::expression() { /* Placeholder implementation */ }
    void Parser::bool_expr() { /* Placeholder implementation */ }
    void Parser::additive_expr() { /* Placeholder implementation */ }
    void Parser::term() { /* Placeholder implementation */ }
    void Parser::factor() { /* Placeholder implementation */ }

} // namespace Compiler