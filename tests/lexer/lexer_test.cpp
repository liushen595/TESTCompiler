#include "Lexer.hpp"
#include <iostream>
#include <cassert>

using namespace Compiler;

void testBasicTokenization() {
    std::cout << "测试基本令牌化..." << std::endl;

    std::string input = "int main() { return 0; }";
    Lexer lexer(input);

    auto tokens = lexer.tokenize();

    std::cout << "找到 " << tokens.size() << " 个令牌:" << std::endl;
    for (const auto& token : tokens) {
        std::cout << tokenTypeToString(token.type) << ": \"" << token.value
            << "\" (行:" << token.line << ", 列:" << token.column << ")" << std::endl;
    }

    // 基本断言
    assert(!tokens.empty());
    assert(tokens.back().type == TokenType::EOF_TOKEN);

    std::cout << "基本令牌化测试通过!" << std::endl;
}

void testIdentifiersAndKeywords() {
    std::cout << "测试标识符和关键字..." << std::endl;

    std::string input = "int variable if else";
    Lexer lexer(input);

    auto tokens = lexer.tokenize();

    // 检查第一个token是关键字 "int"
    assert(tokens[0].type == TokenType::KEYWORD);
    assert(tokens[0].value == "int");

    // 检查第二个token是标识符 "variable"
    assert(tokens[1].type == TokenType::IDENTIFIER);
    assert(tokens[1].value == "variable");

    std::cout << "标识符和关键字测试通过!" << std::endl;
}

void testNumbers() {
    std::cout << "测试数字..." << std::endl;

    std::string input = "123 456.789";
    Lexer lexer(input);

    auto tokens = lexer.tokenize();

    assert(tokens[0].type == TokenType::NUMBER);
    assert(tokens[0].value == "123");

    assert(tokens[1].type == TokenType::NUMBER);
    assert(tokens[1].value == "456.789");

    std::cout << "数字测试通过!" << std::endl;
}

void testStrings() {
    std::cout << "测试字符串..." << std::endl;

    std::string input = "\"hello world\" 'c'";
    Lexer lexer(input);

    auto tokens = lexer.tokenize();

    assert(tokens[0].type == TokenType::STRING);
    assert(tokens[0].value == "hello world");

    assert(tokens[1].type == TokenType::STRING);
    assert(tokens[1].value == "c");

    std::cout << "字符串测试通过!" << std::endl;
}

int main() {
    std::cout << "开始词法分析器测试..." << std::endl;

    try {
        testBasicTokenization();
        testIdentifiersAndKeywords();
        testNumbers();
        testStrings();

        std::cout << "所有测试通过!" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "测试失败: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}