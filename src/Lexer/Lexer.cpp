#include "Lexer.hpp"
#include "DFA_Tables.hpp"
#include <cctype>
#include <unordered_set>
#include <iomanip>  // std::setw, std::left

namespace Compiler {

    // 关键字集合
    static const std::unordered_set<std::string> keywords = {
        "if", "else", "while", "for", "return", "int", "float", "char", "string",
        "bool", "true", "false","read","write"
    };

    std::string tokenTypeToString(TokenType type) {
        switch (type) {
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::NUMBER: return "NUMBER";
        case TokenType::KEYWORD: return "KEYWORD";
        case TokenType::COMPARISON_DOUBLE: return "COMPARISON_DOUBLE";
        case TokenType::COMPARISON_SINGLE: return "COMPARISON_SINGLE";
        case TokenType::DIVISION: return "DIVISION";
        case TokenType::COMMENT_FIRST: return "COMMENT_FIRST";
        case TokenType::COMMENT_LAST: return "COMMENT_LAST";
        case TokenType::SINGLEWORD: return "SINGLEWORD";
        case TokenType::UNKNOWN: return "UNKNOWN";
        default: return "UNKNOWN";
        }
    }

    // 将 DFA 的 token 名称映射到 TokenType 枚举
    TokenType Lexer::mapTokenName(const std::string& tokenName) {
        if (tokenName == "<identifier>") return TokenType::IDENTIFIER;
        if (tokenName == "<number>") return TokenType::NUMBER;
        if (tokenName == "<singleword>") return TokenType::SINGLEWORD;
        if (tokenName == "<comparison_double>") return TokenType::COMPARISON_DOUBLE;
        if (tokenName == "<comparison_single>") return TokenType::COMPARISON_SINGLE;
        if (tokenName == "<division>") return TokenType::DIVISION;
        if (tokenName == "<commentfirst>") return TokenType::COMMENT_FIRST;
        if (tokenName == "<commentlast>") return TokenType::COMMENT_LAST;
        return TokenType::UNKNOWN;
    }

    bool isKeyword(const std::string& identifier) {
        return keywords.find(identifier) != keywords.end();
    }

    // 词法分析器类实现
    // 构造函数
    Lexer::Lexer(const std::string& input)
        : input_(input), position_(0), line_(1), column_(1) {}

    // 获取当前字符
    char Lexer::currentChar() {
        if (position_ >= input_.size()) {
            return '\0';  // EOF
        }
        return input_[position_];
    }

    // 向前查看 offset 个字符
    char Lexer::peekChar(std::size_t offset) {
        std::size_t pos = position_ + offset;
        if (pos >= input_.size()) {
            return '\0';  // EOF
        }
        return input_[pos];
    }

    // 向前移动一个字符
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

    // 跳过空白字符
    void Lexer::skipWhitespace() {
        while (std::isspace(currentChar()) && currentChar() != '\n') {
            advance();
        }
    }

    // 跳过注释
    void Lexer::skipComment() {
        std::size_t commentStartLine = line_;
        std::size_t commentStartColumn = column_;

        // 当遇到 /* 时，继续读取直到遇到 */
        while (currentChar() != '\0') {
            if (currentChar() == '*' && peekChar(1) == '/') {
                // 找到注释结束符 */
                advance(); // 跳过 *
                advance(); // 跳过 /
                return;
            }
            advance();
        }

        // 如果到达这里，说明注释没有结束
        throw LexerException("Unterminated comment", commentStartLine, commentStartColumn);
    }

    // 获取下一个令牌
    Token Lexer::nextToken() {
        // 跳过空白字符
        skipWhitespace();

        // 获取当前字符
        char c = currentChar();

        // EOF 处理
        if (c == '\0') {
            return Token(TokenType::UNKNOWN, "", line_, column_, position_);
        }

        // 换行符处理
        if (c == '\n') {
            // 如果遇到换行符，返回一个单字符令牌
            Token token(TokenType::SINGLEWORD, "\\n", line_, column_, position_);
            advance();
            return token;
        }

        // 使用 DFA 表驱动词法分析
        Token token = runDFA();

        // 特殊处理：遇到注释开始符 /* 时，跳过整个注释
        if (token.type == TokenType::COMMENT_FIRST) {
            skipComment();
            // 递归调用获取下一个有效 token
            return nextToken();
        }

        // 特殊处理: 遇到*/时, 检查是否是孤立的注释结束符
        if (token.type == TokenType::COMMENT_LAST) {
            throw LexerException("Isolated comment end '*/' found", token.line, token.column);
        }

        return token;
    }

    // 重置词法分析器
    void Lexer::reset() {
        position_ = 0;
        line_ = 1;
        column_ = 1;
    }

    // DFA 驱动的词法分析核心方法
    Token Lexer::runDFA() {
        std::size_t startPos = position_;
        std::size_t startLine = line_;
        std::size_t startColumn = column_;
        std::string value;

        int currentState = DFA_START_STATE;
        int lastAcceptState = -1;
        std::size_t lastAcceptPos = position_;
        std::size_t lastAcceptLine = line_;
        std::size_t lastAcceptColumn = column_;
        std::string lastAcceptValue;

        // DFA 主循环：不断读取字符并转移状态
        while (currentChar() != '\0') {
            char c = currentChar();

            // 检查当前状态是否为接受状态
            if (DFA_ACCEPT_STATES.find(currentState) != DFA_ACCEPT_STATES.end()) {
                lastAcceptState = currentState;
                lastAcceptPos = position_;
                lastAcceptLine = line_;
                lastAcceptColumn = column_;
                lastAcceptValue = value;
            }

            // 查找状态转移
            auto stateIt = DFA_TRANSITION_TABLE.find(currentState);
            if (stateIt == DFA_TRANSITION_TABLE.end()) {
                // 当前状态没有转移表，结束
                break;
            }

            auto transIt = stateIt->second.find(c);
            if (transIt == stateIt->second.end()) {
                // 当前字符无法转移，结束
                break;
            }

            // 执行状态转移
            currentState = transIt->second;
            value += c;
            advance();
        }

        // 检查最终状态是否为接受状态
        if (DFA_ACCEPT_STATES.find(currentState) != DFA_ACCEPT_STATES.end()) {
            lastAcceptState = currentState;
            lastAcceptPos = position_;
            lastAcceptLine = line_;
            lastAcceptColumn = column_;
            lastAcceptValue = value;
        }

        // 如果找到了接受状态，回退到最后的接受位置
        if (lastAcceptState != -1) {
            // 回退到最后接受位置
            position_ = lastAcceptPos;
            line_ = lastAcceptLine;
            column_ = lastAcceptColumn;

            // 获取 token 类型
            std::string tokenName = DFA_ACCEPT_STATES.at(lastAcceptState);
            TokenType type = mapTokenName(tokenName);

            // 特殊处理：标识符可能是关键字
            if (type == TokenType::IDENTIFIER && isKeyword(lastAcceptValue)) {
                type = TokenType::KEYWORD;
            }

            return Token(type, lastAcceptValue, startLine, startColumn, startPos);
        }

        // 没有找到接受状态，返回错误
        return Token(TokenType::UNKNOWN, value, startLine, startColumn, startPos);
    }

    // 令牌化整个输入
    std::vector<Token> Lexer::tokenize() {
        std::vector<Token> tokens;

        try {
            while (currentChar() != '\0') {
                Token token = nextToken();

                // 跳过换行符（可选，根据需求）
                if (token.type == TokenType::SINGLEWORD && token.value == "\\n") {
                    continue;
                }

                tokens.push_back(token);

                // 如果遇到未知 token，可以选择报错或继续
                if (token.type == TokenType::UNKNOWN && !token.value.empty()) {
                    // 这里可以记录错误，但继续分析
                    std::cerr << "Warning: Unknown char '" << token.value
                        << "' in line " << token.line
                        << ", column " << token.column << std::endl;
                }
            }
        }
        catch (const LexerException& ex) {
            // 捕获词法分析异常，输出错误信息
            std::cerr << ex.getFullMessage() << std::endl;
            throw; // 重新抛出异常以便上层处理
        }

        return tokens;
    }

} // namespace Compiler

// 输出词法分析结果，格式适合语法分析器使用
void outputLexerResults(const std::vector<Compiler::Token>& tokens, std::ostream& out) {
    // 输出文件头注释
    out << "# 词法分析结果" << std::endl;
    out << "# 格式: TokenType TokenValue Line Column" << std::endl;
    out << "# 总计: " << tokens.size() << " 个词法单元" << std::endl;
    out << std::endl;

    // 输出每个 token，格式：类型 值 行号 列号
    for (const auto& token : tokens) {
        std::string typeStr = Compiler::tokenTypeToString(token.type);
        std::string valueStr = token.value;

        // 处理特殊字符：在值中包含空格或特殊字符时用引号包围
        bool needQuotes = false;
        if (valueStr.empty()) {
            valueStr = "\"\"";
            needQuotes = true;
        }
        else {
            // 检查是否需要引号
            for (char c : valueStr) {
                if (std::isspace(c) || c == ',' || c == '"') {
                    needQuotes = true;
                    break;
                }
            }
        }

        if (needQuotes && valueStr != "\"\"") {
            // 转义已有的引号
            std::string escaped;
            for (char c : valueStr) {
                if (c == '"') {
                    escaped += "\\\"";
                }
                else if (c == '\n') {
                    escaped += "\\n";
                }
                else if (c == '\t') {
                    escaped += "\\t";
                }
                else if (c == '\\') {
                    escaped += "\\\\";
                }
                else {
                    escaped += c;
                }
            }
            valueStr = "\"" + escaped + "\"";
        }

        // 输出格式：类型 值 行号 列号
        out << typeStr << " " << valueStr << " "
            << token.line << " " << token.column << std::endl;
    }

    out << std::endl;
    out << "# 词法分析完成" << std::endl;
}