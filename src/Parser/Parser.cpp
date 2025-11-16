#include "Parser.hpp"
#include <sstream>
#include <algorithm>

namespace Compiler {

    // 构造函数 - 接受token文件路径
    Parser::Parser(const std::string& tokenFile, std::shared_ptr<SemanticAnalyzer> semantic)
        : tokenFilePath_(tokenFile), currentIndex_(0), currentToken_(Token(TokenType::UNKNOWN, "", 0, 0, 0)), semantic_(semantic) {
        if (!loadTokensFromFile(tokenFile)) {
            throw ParseException("无法加载token文件: " + tokenFile, 0, 0);
        }
        if (!tokens_.empty()) {
            currentToken_ = tokens_[0];
        }
    }

    // 构造函数 - 接受token向量
    Parser::Parser(const std::vector<Token>& tokens, std::shared_ptr<SemanticAnalyzer> semantic)
        : tokens_(tokens), currentIndex_(0), currentToken_(Token(TokenType::UNKNOWN, "", 0, 0, 0)), semantic_(semantic) {
        if (!tokens_.empty()) {
            currentToken_ = tokens_[0];
        }
    }

    // 从token文件读取tokens
    bool Parser::loadTokensFromFile(const std::string& tokenFile) {
        std::ifstream file(tokenFile);
        if (!file.is_open()) {
            return false;
        }

        std::string line;
        while (std::getline(file, line)) {
            // 跳过注释行和空行
            if (line.empty() || line[0] == '#') {
                continue;
            }

            std::istringstream iss(line);
            std::string typeStr, value;
            std::size_t lineNum, columnNum;

            if (!(iss >> typeStr >> std::ws)) {
                continue; // 读取类型失败，跳过
            }

            if (!(iss >> value)) {
                continue; // 读取值失败，跳过
            }

            if (!(iss >> lineNum >> columnNum)) {
                continue; // 读取位置失败，跳过
            }

            TokenType type = TokenType::UNKNOWN;
            if (typeStr == "IDENTIFIER") type = TokenType::IDENTIFIER;
            else if (typeStr == "NUMBER") type = TokenType::NUMBER;
            else if (typeStr == "KEYWORD") type = TokenType::KEYWORD;
            else if (typeStr == "COMPARISON_DOUBLE") type = TokenType::COMPARISON_DOUBLE;
            else if (typeStr == "COMPARISON_SINGLE") type = TokenType::COMPARISON_SINGLE;
            else if (typeStr == "DIVISION") type = TokenType::DIVISION;
            else if (typeStr == "SINGLEWORD") type = TokenType::SINGLEWORD;

            tokens_.emplace_back(type, value, lineNum, columnNum, 0);
        }

        file.close();
        return true;
    }

    // 获取下一个token
    void Parser::advance() {
        if (currentIndex_ < tokens_.size() - 1) {
            currentIndex_++;
            currentToken_ = tokens_[currentIndex_];
        }
        else {
            currentIndex_ = tokens_.size(); // 移动到末尾
            currentToken_ = Token(TokenType::UNKNOWN, "", 0, 0, 0); // 设置为无效token
        }
    }

    // 检查当前token是否匹配期望的值
    bool Parser::match(const std::string& expected) {
        return currentToken_.value == expected;
    }

    // 消费一个期望的token
    void Parser::consume(const std::string& expected) {
        if (currentIndex_ >= tokens_.size()) {
            throw ParseException("Unexpected end of input, expected '" + expected + "'", getCurrentLine(), getCurrentColumn());
        }
        if (!match(expected)) {
            throw ParseException("Expect '" + expected + "', but read '" + currentToken_.value + "'", getCurrentLine(), getCurrentColumn());
        }
        advance();
    }

    // 获取当前token位置信息
    std::size_t Parser::getCurrentLine() const {
        if (currentIndex_ < tokens_.size()) {
            return currentToken_.line;
        }
        return 0;
    }

    std::size_t Parser::getCurrentColumn() const {
        if (currentIndex_ < tokens_.size()) {
            return currentToken_.column;
        }
        return 0;
    }

    // 执行语法分析
    void Parser::parse() {
        try {
            program();
            std::cout << "==语法分析结果==" << std::endl;
            std::cout << "语法分析成功" << std::endl;
        }
        catch (const ParseException& e) {
            throw e; // 重新抛出异常
        }
        catch (const SemanticException& e) {
            throw e; // 重新抛出异常
        }
    }

    // <program> → { <declaration_list> <statement_list> }
    void Parser::program() {
        // 期望: { declaration_list statement_list }

        consume("{");

        // 初始化语义状态
        semantic_->reset();

        declaration_list();

        statement_list();

        consume("}");

        semantic_->emitSTOP();
    }

    // <declaration_list> → <declaration_list> <declaration_stat> | ε
    void Parser::declaration_list() {
        // 左递归改写为右递归或循环

        while (match("int")) {
            declaration_stat();
        }
    }

    // <declaration_stat> → int ID;
    void Parser::declaration_stat() {
        consume("int");

        if (currentIndex_ >= tokens_.size() || currentToken_.type != TokenType::IDENTIFIER) {
            throw ParseException("Expect identifier", getCurrentLine(), getCurrentColumn());
        }
        // 记录声明的变量名与位置信息
        std::string varName = currentToken_.value;
        std::size_t idLine = currentToken_.line;
        std::size_t idCol = currentToken_.column;
        // 声明插入符号表（可能抛语义错误：重复定义）
        semantic_->declareVariable(varName, idLine, idCol);
        advance(); // 消费ID

        consume(";");
    }

    // <statement_list> → <statement_list> <statement> | ε
    void Parser::statement_list() {
        // 左递归改写为右递归或循环

        while (match("if") || match("while") || match("for") ||
            match("read") || match("write") || match("{") ||
            (currentIndex_ < tokens_.size() && currentToken_.type == TokenType::IDENTIFIER) || match(";")) {
            statement();
        }
    }

    // <statement> → <if_stat> | <while_stat> | <for_stat> | <read_stat> | <write_stat> | <compound_stat> | <expression_stat>
    void Parser::statement() {
        if (match("if")) {
            if_stat();
        }
        else if (match("while")) {
            while_stat();
        }
        else if (match("for")) {
            for_stat();
        }
        else if (match("read")) {
            read_stat();
        }
        else if (match("write")) {
            write_stat();
        }
        else if (match("{")) {
            compound_stat();
        }
        else {
            expression_stat();
        }
    }

    // <if_stat> → if(<expression>) <statement> [else <statement>]
    void Parser::if_stat() {
        consume("if");
        consume("(");

        expression(); // 结果在栈顶，非零为真

        consume(")");

        // 生成分支控制
        std::string L1 = semantic_->newLabel();
        std::string L2 = semantic_->newLabel();
        semantic_->emitBRF(L1);

        statement(); // then

        semantic_->emitBR(L2);
        semantic_->emitLabel(L1);

        if (match("else")) { // else 可选
            advance();
            statement();
        }
        semantic_->emitLabel(L2);
    }

    // <while_stat> → while(<expression>) <statement>
    void Parser::while_stat() {
        consume("while");
        consume("(");

        // while 起始标签
        std::string L1 = semantic_->newLabel();
        std::string L2 = semantic_->newLabel();
        semantic_->emitLabel(L1);

        expression();
        consume(")");
        semantic_->emitBRF(L2);

        statement();
        semantic_->emitBR(L1);
        semantic_->emitLabel(L2);
    }

    // <for_stat> → for(<expression>;<expression>;<expression>) <statement>
    void Parser::for_stat() {
        consume("for");
        consume("(");

        // init
        expression();
        semantic_->emitPop();
        consume(";");

        // cond
        std::string L1 = semantic_->newLabel();
        std::string L2 = semantic_->newLabel();
        std::string L3 = semantic_->newLabel();
        std::string L4 = semantic_->newLabel();
        semantic_->emitLabel(L1);
        expression();
        semantic_->emitBRF(L2);
        semantic_->emitBR(L3);
        consume(";");

        // step
        semantic_->emitLabel(L4);
        expression();
        semantic_->emitPop();
        semantic_->emitBR(L1);
        consume(")");

        // body
        semantic_->emitLabel(L3);
        statement();
        semantic_->emitBR(L4);
        semantic_->emitLabel(L2);
    }

    // <read_stat> → read ID;
    void Parser::read_stat() {
        consume("read");

        if (currentIndex_ >= tokens_.size() || currentToken_.type != TokenType::IDENTIFIER) {
            throw ParseException("Expect identifier", getCurrentLine(), getCurrentColumn());
        }
        {
            std::string name = currentToken_.value;
            std::size_t l = currentToken_.line, c = currentToken_.column;
            int addr = semantic_->lookupAddress(name);
            if (addr < 0) {
                throw SemanticException("Use of undeclared variable '" + name + "'", l, c);
            }
            semantic_->emitIN();
            semantic_->emitStore(addr);
            semantic_->emitPop();
        }
        advance();

        consume(";");
    }

    // <write_stat> → write <expression>;
    void Parser::write_stat() {
        consume("write");

        expression();
        semantic_->emitOUT();

        consume(";");
    }

    // <compound_stat> → { <statement_list> }
    void Parser::compound_stat() {
        consume("{");

        statement_list();

        consume("}");
    }

    // <expression_stat> → <expression>; | ;
    void Parser::expression_stat() {
        if (match(";")) {
            advance(); // 消费单独的;
            return;
        }

        expression();
        semantic_->emitPop();

        consume(";");
    }

    // <expression> → ID=<bool_expr> | <bool_expr>
    void Parser::expression() {
        // 需要超前读一个符号来解决首符号集相交问题

        if (currentIndex_ < tokens_.size() && currentToken_.type == TokenType::IDENTIFIER) {
            // 保存当前位置，用于回退
            std::size_t savedIndex = currentIndex_;
            Token savedToken = currentToken_;

            advance(); // 读取ID后的下一个符号

            if (match("=")) {
                // 赋值表达式：ID = <bool_expr>
                std::string name = savedToken.value;
                std::size_t l = savedToken.line, c = savedToken.column;
                int addr = semantic_->lookupAddress(name);
                if (addr < 0) {
                    throw SemanticException("Assignment to undeclared variable '" + name + "'", l, c);
                }
                advance(); // 消费 =
                bool_expr();
                semantic_->emitStore(addr);
            }
            else {
                // 布尔表达式：回退并分析为bool_expr
                currentIndex_ = savedIndex;
                currentToken_ = savedToken;
                bool_expr();
            }
        }
        else {
            bool_expr();
        }
    }

    // <bool_expr> → <additive_expr> | <additive_expr>(>|<|>=|<=|==|!=)<additive_expr>
    void Parser::bool_expr() {
        additive_expr();

        // 检查是否有比较操作符
        if (match(">") || match("<") || match(">=") ||
            match("<=") || match("==") || match("!=")) {
            std::string op = currentToken_.value;
            advance(); // 消费比较操作符

            additive_expr();

            if (op == ">") semantic_->emitGT();
            else if (op == "<") semantic_->emitLES();
            else if (op == ">=") semantic_->emitGE();
            else if (op == "<=") semantic_->emitLE();
            else if (op == "==") semantic_->emitEQ();
            else if (op == "!=") semantic_->emitNOTEQ();
        }
    }

    // <additive_expr> → <term>{(+|-)<term>}
    void Parser::additive_expr() {
        term();

        while (match("+") || match("-")) {
            std::string op = currentToken_.value;
            advance(); // 消费操作符
            term();
            if (op == "+") semantic_->emitAdd();
            else semantic_->emitSub();
        }
    }

    // <term> → <factor>{(*|/)<factor>}
    void Parser::term() {
        factor();

        while (match("*") || match("/")) {
            std::string op = currentToken_.value;
            advance(); // 消费操作符
            factor();
            if (op == "*") semantic_->emitMult();
            else semantic_->emitDiv();
        }
    }

    // <factor> → (<expression>) | ID | NUM
    void Parser::factor() {
        if (match("(")) {
            advance(); // 消费(

            expression();

            consume(")");
        }
        else if (currentIndex_ < tokens_.size() &&
            (currentToken_.type == TokenType::IDENTIFIER ||
                currentToken_.type == TokenType::NUMBER)) {
            if (currentToken_.type == TokenType::IDENTIFIER) {
                std::string name = currentToken_.value;
                std::size_t l = currentToken_.line, c = currentToken_.column;
                int addr = semantic_->lookupAddress(name);
                if (addr < 0) {
                    throw SemanticException("Use of undeclared variable '" + name + "'", l, c);
                }
                semantic_->emitLoad(addr);
            }
            else { // NUMBER
                semantic_->emitLoadI(currentToken_.value);
            }
            advance(); // 消费ID或NUM
        }
        else {
            throw ParseException("Expect factor (ID, NUM, or (expression))", getCurrentLine(), getCurrentColumn());
        }
    }

} // namespace Compiler