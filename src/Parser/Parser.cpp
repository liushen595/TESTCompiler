#include "Parser.hpp"
#include <sstream>
#include <algorithm>

namespace Compiler {

    // 构造函数 - 接受词法分析器智能指针
    Parser::Parser(std::shared_ptr<Lexer> lexer)
        : lexer_(lexer), currentToken_(TokenType::EOF_TOKEN, "", 0, 0, 0), astRoot_(nullptr) {
        if (lexer_ == nullptr) {
            throw ParseException("Lexer cannot be null", 0, 0);
        }
        // 获取第一个token
        advance();
    }

    // 构造函数 - 从输入字符串创建
    Parser::Parser(const std::string& input)
        : lexer_(std::make_shared<Lexer>(input)), currentToken_(TokenType::EOF_TOKEN, "", 0, 0, 0), astRoot_(nullptr) {
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
                throw ex; // 重新抛出异常以便上层处理
            }
            catch (const std::exception& ex) {
                throw ParseException("Unexpected error during tokenization: " + std::string(ex.what()),
                    lexer_->getLine(), lexer_->getColumn());
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

    // 获取当前token位置信息
    std::size_t Parser::getCurrentLine() const {
        return currentToken_.line;
    }

    std::size_t Parser::getCurrentColumn() const {
        return currentToken_.column;
    }

    std::string Parser::tokenToTerminal(const Token& token) {
        switch (token.type) {
        case TokenType::IDENTIFIER:
            return "IDENTIFIER";
        case TokenType::NUMBER:
            return "NUMBER";
        case TokenType::KEYWORD:
            // 关键字直接返回其值
            return token.value;
        case TokenType::COMPARISON_DOUBLE:
        case TokenType::COMPARISON_SINGLE:
        case TokenType::DIVISION:
        case TokenType::SINGLEWORD:
            // 运算符和分隔符返回其值
            return token.value;
        case TokenType::EOF_TOKEN:
            return "$";
        default:
            return token.value;
        }
    }

    void Parser::initializeStack() {
        // 清空栈
        while (!parseStack.empty()) {
            parseStack.pop();
        }
        while (!astStack.empty()) {
            astStack.pop();
        }
        // 压入$
        parseStack.push({"$", SymbolType::TERMINAL});
        // 初始化栈顶为起始符号
        parseStack.push({"<program>", SymbolType::NON_TERMINAL});
    }

    int Parser::getProductionIndex(const std::string& nonTerminal, const std::string& terminal) {
        // 特殊处理: <else_part> 的 if-else 冲突
        // 当非终结符是 <else_part> 时，根据当前终结符决定使用哪个产生式
        if (nonTerminal == "<else_part>") {
            if (terminal == "else") {
                // 当前是 else 关键字，使用非空产生式: <else_part> -> else <statement>
                // 需要找到对应的产生式索引
                for (size_t i = 0; i < PRODUCTIONS.size(); ++i) {
                    const auto& prod = PRODUCTIONS[i];
                    if (prod.left == "<else_part>" &&
                        !prod.right.empty() &&
                        prod.right[0].first == "else") {
                        return static_cast<int>(i);
                    }
                }
            }
            else {
                // 当前不是 else，使用 ε 产生式: <else_part> -> ε
                for (size_t i = 0; i < PRODUCTIONS.size(); ++i) {
                    const auto& prod = PRODUCTIONS[i];
                    if (prod.left == "<else_part>" &&
                        prod.right.size() == 1 &&
                        prod.right[0].first == "ε") {
                        return static_cast<int>(i);
                    }
                }
            }
        }

        // 正常查表
        auto ntIt = NON_TERMINALS.find(nonTerminal);
        auto tIt = TERMINALS.find(terminal);

        if (ntIt == NON_TERMINALS.end() || tIt == TERMINALS.end()) {
            return -1;
        }

        auto key = std::make_pair(ntIt->second, tIt->second);
        auto it = PARSING_TABLE.find(key);

        return (it != PARSING_TABLE.end()) ? it->second : -1;
    }

    // AST构造方法 - 根据产生式规约构造AST节点
    void Parser::buildASTNode(const Production& prod) {
        std::string leftSymbol = prod.left;
        size_t rightSize = prod.right.size();
        bool isEpsilon = (rightSize == 1 && prod.right[0].first == "ε");

        // 处理ε产生式
        if (isEpsilon) {
            astStack.push(std::make_shared<EmptyNode>());
            return;
        }

        // 从栈中弹出与产生式右部符号数量相同的AST节点
        std::vector<std::shared_ptr<ASTNode>> children;
        for (size_t i = 0; i < rightSize; ++i) {
            if (!astStack.empty()) {
                children.push_back(astStack.top());
                astStack.pop();
            }
        }
        // 由于栈是后进先出，需要反转
        std::reverse(children.begin(), children.end());

        // 根据产生式左部构造相应的AST节点
        std::shared_ptr<ASTNode> node = nullptr;

        if (leftSymbol == "<program>") {
            // <program> → { <declaration_list> <statement_list> }
            // children: {, decl_list, stmt_list, }
            node = std::make_shared<ProgramNode>(
                children.size() > 1 ? children[1] : nullptr,
                children.size() > 2 ? children[2] : nullptr
            );
        }
        else if (leftSymbol == "<declaration_list>") {
            // <declaration_list> → <declaration_stat> <declaration_list> | ε
            if (children.size() >= 2) {
                auto listNode = std::make_shared<DeclarationListNode>();
                listNode->addDeclaration(children[0]);
                // 合并子列表
                if (children[1] && children[1]->getType() == ASTNodeType::DECLARATION_LIST) {
                    auto subList = std::dynamic_pointer_cast<DeclarationListNode>(children[1]);
                    for (const auto& decl : subList->getDeclarations()) {
                        listNode->addDeclaration(decl);
                    }
                }
                node = listNode;
            }
            else {
                node = std::make_shared<DeclarationListNode>();
            }
        }
        else if (leftSymbol == "<declaration_stat>") {
            // <declaration_stat> → int IDENTIFIER ;
            // children: int, IDENTIFIER, ;
            if (children.size() >= 2) {
                auto identNode = std::dynamic_pointer_cast<IdentifierNode>(children[1]);
                if (identNode) {
                    node = std::make_shared<DeclarationNode>(
                        "int", identNode->getName(),
                        identNode->getLine(), identNode->getColumn()
                    );
                }
            }
        }
        else if (leftSymbol == "<statement_list>") {
            // <statement_list> → <statement> <statement_list> | ε
            if (children.size() >= 2) {
                auto listNode = std::make_shared<StatementListNode>();
                listNode->addStatement(children[0]);
                // 合并子列表
                if (children[1] && children[1]->getType() == ASTNodeType::STATEMENT_LIST) {
                    auto subList = std::dynamic_pointer_cast<StatementListNode>(children[1]);
                    for (const auto& stmt : subList->getStatements()) {
                        listNode->addStatement(stmt);
                    }
                }
                node = listNode;
            }
            else {
                node = std::make_shared<StatementListNode>();
            }
        }
        else if (leftSymbol == "<statement>") {
            // <statement> → <if_stat> | <other_stat>
            node = children.empty() ? nullptr : children[0];
        }
        else if (leftSymbol == "<if_stat>") {
            // <if_stat> → if ( <expression> ) <statement> <else_part>
            // children: if, (, expr, ), stmt, else_part
            std::shared_ptr<ASTNode> elseBranch = nullptr;
            if (children.size() >= 6 && children[5] &&
                children[5]->getType() != ASTNodeType::EMPTY) {
                elseBranch = children[5];
            }
            node = std::make_shared<IfStatementNode>(
                children.size() > 2 ? children[2] : nullptr,
                children.size() > 4 ? children[4] : nullptr,
                elseBranch
            );
        }
        else if (leftSymbol == "<else_part>") {
            // <else_part> → else <statement> | ε
            if (children.size() >= 2) {
                node = children[1]; // 直接返回statement
            }
            else {
                node = std::make_shared<EmptyNode>();
            }
        }
        else if (leftSymbol == "<other_stat>") {
            // <other_stat> → <while_stat> | <for_stat> | ...
            node = children.empty() ? nullptr : children[0];
        }
        else if (leftSymbol == "<while_stat>") {
            // <while_stat> → while ( <expression> ) <statement>
            // children: while, (, expr, ), stmt
            node = std::make_shared<WhileStatementNode>(
                children.size() > 2 ? children[2] : nullptr,
                children.size() > 4 ? children[4] : nullptr
            );
        }
        else if (leftSymbol == "<for_stat>") {
            // <for_stat> → for ( <expression> ; <expression> ; <expression> ) <statement>
            // children: for, (, expr1, ;, expr2, ;, expr3, ), stmt
            node = std::make_shared<ForStatementNode>(
                children.size() > 2 ? children[2] : nullptr,
                children.size() > 4 ? children[4] : nullptr,
                children.size() > 6 ? children[6] : nullptr,
                children.size() > 8 ? children[8] : nullptr
            );
        }
        else if (leftSymbol == "<write_stat>") {
            // <write_stat> → write <expression> ;
            // children: write, expr, ;
            node = std::make_shared<WriteStatementNode>(
                children.size() > 1 ? children[1] : nullptr
            );
        }
        else if (leftSymbol == "<read_stat>") {
            // <read_stat> → read IDENTIFIER ;
            // children: read, IDENTIFIER, ;
            if (children.size() >= 2) {
                auto identNode = std::dynamic_pointer_cast<IdentifierNode>(children[1]);
                if (identNode) {
                    node = std::make_shared<ReadStatementNode>(
                        identNode->getName(),
                        identNode->getLine(), identNode->getColumn()
                    );
                }
            }
        }
        else if (leftSymbol == "<compound_stat>") {
            // <compound_stat> → { <statement_list> }
            // children: {, stmt_list, }
            node = std::make_shared<CompoundStatementNode>(
                children.size() > 1 ? children[1] : nullptr
            );
        }
        else if (leftSymbol == "<expression_stat>") {
            // <expression_stat> → <expression> ; | ;
            if (children.size() >= 2) {
                node = std::make_shared<ExpressionStatementNode>(children[0]);
            }
            else {
                node = std::make_shared<ExpressionStatementNode>(nullptr);
            }
        }
        else if (leftSymbol == "<expression>") {
            // <expression> → <additive_expr> <expression_prime>
            if (children.size() >= 2 && children[1] &&
                children[1]->getType() != ASTNodeType::EMPTY) {
                // 有expression_prime，构造二元表达式
                auto exprPrime = std::dynamic_pointer_cast<BinaryExpressionNode>(children[1]);
                if (exprPrime) {
                    // 将左操作数设置为additive_expr
                    node = std::make_shared<BinaryExpressionNode>(
                        exprPrime->getOperator(),
                        children[0],
                        exprPrime->getRight()
                    );
                }
                else {
                    node = children[0];
                }
            }
            else {
                node = children.empty() ? nullptr : children[0];
            }
        }
        else if (leftSymbol == "<expression_prime>") {
            // <expression_prime> → <rel_op> <additive_expr> | = <expression> | ε
            if (children.size() >= 2) {
                std::string op;
                if (auto binOp = std::dynamic_pointer_cast<BinaryExpressionNode>(children[0])) {
                    op = binOp->getOperator();
                }
                else if (auto identNode = std::dynamic_pointer_cast<IdentifierNode>(children[0])) {
                    op = identNode->getName(); // 运算符作为标识符存储
                }
                node = std::make_shared<BinaryExpressionNode>(
                    op.empty() ? "=" : op,
                    nullptr, // 左操作数稍后设置
                    children[1]
                );
            }
            else {
                node = std::make_shared<EmptyNode>();
            }
        }
        else if (leftSymbol == "<rel_op>") {
            // <rel_op> → > | < | >= | <= | == | !=
            // 运算符作为标识符节点暂存
            if (!children.empty()) {
                node = children[0];
            }
        }
        else if (leftSymbol == "<additive_expr>") {
            // <additive_expr> → <term> <additive_expr_prime>
            if (children.size() >= 2 && children[1] &&
                children[1]->getType() != ASTNodeType::EMPTY) {
                auto exprPrime = std::dynamic_pointer_cast<BinaryExpressionNode>(children[1]);
                if (exprPrime) {
                    node = std::make_shared<BinaryExpressionNode>(
                        exprPrime->getOperator(),
                        children[0],
                        exprPrime->getRight()
                    );
                }
                else {
                    node = children[0];
                }
            }
            else {
                node = children.empty() ? nullptr : children[0];
            }
        }
        else if (leftSymbol == "<additive_expr_prime>") {
            // <additive_expr_prime> → + <term> <additive_expr_prime> | - <term> <additive_expr_prime> | ε
            if (children.size() >= 3 && children[2] &&
                children[2]->getType() != ASTNodeType::EMPTY) {
                // 需要构造左结合的表达式树
                auto rightPrime = std::dynamic_pointer_cast<BinaryExpressionNode>(children[2]);
                std::string op;
                if (auto identNode = std::dynamic_pointer_cast<IdentifierNode>(children[0])) {
                    op = identNode->getName();
                }
                auto leftBinary = std::make_shared<BinaryExpressionNode>(
                    op, nullptr, children[1]
                );
                if (rightPrime) {
                    node = std::make_shared<BinaryExpressionNode>(
                        rightPrime->getOperator(),
                        leftBinary,
                        rightPrime->getRight()
                    );
                }
                else {
                    node = leftBinary;
                }
            }
            else if (children.size() >= 2) {
                std::string op;
                if (auto identNode = std::dynamic_pointer_cast<IdentifierNode>(children[0])) {
                    op = identNode->getName();
                }
                node = std::make_shared<BinaryExpressionNode>(
                    op, nullptr, children[1]
                );
            }
            else {
                node = std::make_shared<EmptyNode>();
            }
        }
        else if (leftSymbol == "<term>") {
            // <term> → <factor> <term_prime>
            if (children.size() >= 2 && children[1] &&
                children[1]->getType() != ASTNodeType::EMPTY) {
                auto termPrime = std::dynamic_pointer_cast<BinaryExpressionNode>(children[1]);
                if (termPrime) {
                    node = std::make_shared<BinaryExpressionNode>(
                        termPrime->getOperator(),
                        children[0],
                        termPrime->getRight()
                    );
                }
                else {
                    node = children[0];
                }
            }
            else {
                node = children.empty() ? nullptr : children[0];
            }
        }
        else if (leftSymbol == "<term_prime>") {
            // <term_prime> → * <factor> <term_prime> | / <factor> <term_prime> | ε
            if (children.size() >= 3 && children[2] &&
                children[2]->getType() != ASTNodeType::EMPTY) {
                auto rightPrime = std::dynamic_pointer_cast<BinaryExpressionNode>(children[2]);
                std::string op;
                if (auto identNode = std::dynamic_pointer_cast<IdentifierNode>(children[0])) {
                    op = identNode->getName();
                }
                auto leftBinary = std::make_shared<BinaryExpressionNode>(
                    op, nullptr, children[1]
                );
                if (rightPrime) {
                    node = std::make_shared<BinaryExpressionNode>(
                        rightPrime->getOperator(),
                        leftBinary,
                        rightPrime->getRight()
                    );
                }
                else {
                    node = leftBinary;
                }
            }
            else if (children.size() >= 2) {
                std::string op;
                if (auto identNode = std::dynamic_pointer_cast<IdentifierNode>(children[0])) {
                    op = identNode->getName();
                }
                node = std::make_shared<BinaryExpressionNode>(
                    op, nullptr, children[1]
                );
            }
            else {
                node = std::make_shared<EmptyNode>();
            }
        }
        else if (leftSymbol == "<factor>") {
            // <factor> → ( <expression> ) | IDENTIFIER | NUMBER
            if (children.size() == 3) {
                // ( <expression> )
                node = children[1];
            }
            else if (children.size() == 1) {
                node = children[0];
            }
        }
        else {
            // 默认：取第一个子节点
            node = children.empty() ? nullptr : children[0];
        }

        // 将构造的节点压入AST栈
        if (node) {
            astStack.push(node);
        }
        else {
            astStack.push(std::make_shared<EmptyNode>());
        }
    }

    // 执行语法分析
    void Parser::parse() {
        std::cout << "Parser::parse() - Ready for LL syntax analysis" << std::endl;

        // 初始化栈
        initializeStack();

        // 分析主循环
        while (!parseStack.empty()) {
            std::pair<std::string, SymbolType> stackTop = parseStack.top();
            std::string currentTerminal = tokenToTerminal(currentToken_);

            std::cerr << "\033[34m[DEBUG] Stack top: " << stackTop.first
                << ", Current token: " << currentTerminal
                << " (" << currentToken_.value << ")" << "\033[0m" << std::endl;

            // 检查是否是规约标记（以@开头的特殊符号）
            if (stackTop.first.substr(0, 1) == "@") {
                // 这是一个规约标记，执行规约
                parseStack.pop();
                int prodIdx = std::stoi(stackTop.first.substr(1));

                std::cerr << "\033[34m[DEBUG] Reducing by production " << prodIdx << "\033[0m" << std::endl;

                // 调用buildASTNode构造AST节点
                const Production& prod = PRODUCTIONS[prodIdx];
                buildASTNode(prod);
                continue;
            }

            // 情况1: 栈顶是终结符
            if (stackTop.second == SymbolType::TERMINAL) {
                if (stackTop.first == "$" && currentTerminal == "$") {
                    // 分析成功
                    std::cout << "Parsing completed successfully!" << std::endl;
                    parseStack.pop();
                    // 保存AST根节点
                    if (!astStack.empty()) {
                        astRoot_ = astStack.top();
                        astStack.pop();
                    }
                    break;
                }
                else if (stackTop.first == currentTerminal) {
                    // 匹配,弹出栈顶并前进
                    std::cerr << "\033[34m[DEBUG] Matched: " << stackTop.first << "\033[0m" << std::endl;
                    parseStack.pop();

                    // 为终结符创建AST叶子节点
                    std::shared_ptr<ASTNode> leafNode = nullptr;
                    if (currentToken_.type == TokenType::IDENTIFIER) {
                        leafNode = std::make_shared<IdentifierNode>(
                            currentToken_.value, currentToken_.line, currentToken_.column
                        );
                    }
                    else if (currentToken_.type == TokenType::NUMBER) {
                        leafNode = std::make_shared<NumberLiteralNode>(
                            currentToken_.value, currentToken_.line, currentToken_.column
                        );
                    }
                    else if (currentToken_.type == TokenType::KEYWORD ||
                        currentToken_.type == TokenType::COMPARISON_DOUBLE ||
                        currentToken_.type == TokenType::COMPARISON_SINGLE ||
                        currentToken_.type == TokenType::SINGLEWORD) {
                        // 运算符和关键字作为标识符节点（稍后会被运算符节点使用）
                        leafNode = std::make_shared<IdentifierNode>(
                            currentToken_.value, currentToken_.line, currentToken_.column
                        );
                    }

                    if (leafNode) {
                        astStack.push(leafNode);
                    }

                    advance();
                }
                else {
                    // 错误:不匹配
                    throw ParseException(
                        "expected '" + stackTop.first +
                        "' but found '" + currentTerminal + "'",
                        getCurrentLine(), getCurrentColumn()
                    );
                }
            }
            // 情况2: 栈顶是非终结符
            else if (stackTop.second == SymbolType::NON_TERMINAL) {
                // 查询分析表
                int productionIdx = getProductionIndex(stackTop.first, currentTerminal);

                if (productionIdx == -1) {
                    // 错误:分析表中没有对应项
                    throw ParseException(
                        "unexpected token '" + currentTerminal,
                        getCurrentLine(), getCurrentColumn()
                    );
                }

                // 应用产生式
                const Production& prod = PRODUCTIONS[productionIdx];
                std::cerr << "\033[34m[DEBUG] Applying production " << productionIdx << ": " << prod.left << " -> ";
                for (const auto& sym : prod.right) {
                    std::cerr << sym.first << " ";
                }
                std::cerr << "\033[0m" << std::endl;

                parseStack.pop(); // 弹出非终结符

                // 压入规约标记（在产生式右部之后执行规约）
                parseStack.push({"@" + std::to_string(productionIdx), SymbolType::TERMINAL});

                // 反向压入产生式右侧符号
                bool isEpsilon = (prod.right.size() == 1 && prod.right[0].first == "ε");

                if (!isEpsilon) {
                    for (auto it = prod.right.rbegin(); it != prod.right.rend(); ++it) {
                        parseStack.push(*it);
                    }
                }
            }
            else {
                // 错误: 不应出现EPSILON在栈顶的情况
                throw ParseException(
                    "Internal error: EPSILON found on parse stack",
                    getCurrentLine(), getCurrentColumn()
                );
            }
        }

        std::cout << "Syntax analysis completed" << std::endl;

        std::cerr << "\033[33m[DEBUG] Final AST stack size: " << astStack.size() << "\033[0m" << std::endl;
    }

    void Parser::printAST(std::ostream& os) const {
        if (astRoot_) {
            Compiler::printAST(astRoot_, os);
        }
        else {
            os << "AST is empty (parsing not performed or failed)" << std::endl;
        }
    }

} // namespace Compiler