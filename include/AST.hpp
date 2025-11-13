#ifndef AST_HPP
#define AST_HPP

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <sstream>

namespace Compiler {

    // ==================== AST节点类型枚举 ====================
    enum class ASTNodeType {
        // 程序根节点
        PROGRAM,

        // 声明节点
        DECLARATION_LIST,
        DECLARATION,

        // 语句节点
        STATEMENT_LIST,
        IF_STATEMENT,
        WHILE_STATEMENT,
        FOR_STATEMENT,
        COMPOUND_STATEMENT,
        EXPRESSION_STATEMENT,
        READ_STATEMENT,
        WRITE_STATEMENT,

        // 表达式节点
        BINARY_EXPRESSION,      // 二元表达式 (运算符: +, -, *, /, >, <, >=, <=, ==, !=, =)
        UNARY_EXPRESSION,       // 一元表达式
        IDENTIFIER_EXPRESSION,  // 标识符
        NUMBER_LITERAL,         // 数字字面量

        // 其他
        EMPTY                   // 空节点(用于ε产生式)
    };

    // ==================== AST节点基类 ====================
    class ASTNode {
    protected:
        ASTNodeType type_;
        std::size_t line_;
        std::size_t column_;

    public:
        ASTNode(ASTNodeType type, std::size_t line = 0, std::size_t column = 0)
            : type_(type), line_(line), column_(column) {}

        virtual ~ASTNode() = default;

        ASTNodeType getType() const { return type_; }
        std::size_t getLine() const { return line_; }
        std::size_t getColumn() const { return column_; }

        // 纯虚函数：打印节点信息（用于可视化）
        virtual void print(std::ostream& os, int indent = 0) const = 0;

        // 获取节点类型名称
        virtual std::string getNodeTypeName() const = 0;
    };

    // ==================== 程序根节点 ====================
    class ProgramNode : public ASTNode {
    private:
        std::shared_ptr<ASTNode> declarations_;
        std::shared_ptr<ASTNode> statements_;

    public:
        ProgramNode(std::shared_ptr<ASTNode> decls, std::shared_ptr<ASTNode> stmts)
            : ASTNode(ASTNodeType::PROGRAM), declarations_(decls), statements_(stmts) {}

        std::shared_ptr<ASTNode> getDeclarations() const { return declarations_; }
        std::shared_ptr<ASTNode> getStatements() const { return statements_; }

        void print(std::ostream& os, int indent = 0) const override;
        std::string getNodeTypeName() const override { return "Program"; }
    };

    // ==================== 声明列表节点 ====================
    class DeclarationListNode : public ASTNode {
    private:
        std::vector<std::shared_ptr<ASTNode>> declarations_;

    public:
        DeclarationListNode() : ASTNode(ASTNodeType::DECLARATION_LIST) {}

        void addDeclaration(std::shared_ptr<ASTNode> decl) {
            declarations_.push_back(decl);
        }

        const std::vector<std::shared_ptr<ASTNode>>& getDeclarations() const {
            return declarations_;
        }

        void print(std::ostream& os, int indent = 0) const override;
        std::string getNodeTypeName() const override { return "DeclarationList"; }
    };

    // ==================== 声明节点 ====================
    class DeclarationNode : public ASTNode {
    private:
        std::string varType_;      // 变量类型 (如 "int")
        std::string varName_;      // 变量名

    public:
        DeclarationNode(const std::string& type, const std::string& name,
            std::size_t line, std::size_t column)
            : ASTNode(ASTNodeType::DECLARATION, line, column),
            varType_(type), varName_(name) {}

        std::string getVarType() const { return varType_; }
        std::string getVarName() const { return varName_; }

        void print(std::ostream& os, int indent = 0) const override;
        std::string getNodeTypeName() const override { return "Declaration"; }
    };

    // ==================== 语句列表节点 ====================
    class StatementListNode : public ASTNode {
    private:
        std::vector<std::shared_ptr<ASTNode>> statements_;

    public:
        StatementListNode() : ASTNode(ASTNodeType::STATEMENT_LIST) {}

        void addStatement(std::shared_ptr<ASTNode> stmt) {
            statements_.push_back(stmt);
        }

        const std::vector<std::shared_ptr<ASTNode>>& getStatements() const {
            return statements_;
        }

        void print(std::ostream& os, int indent = 0) const override;
        std::string getNodeTypeName() const override { return "StatementList"; }
    };

    // ==================== If语句节点 ====================
    class IfStatementNode : public ASTNode {
    private:
        std::shared_ptr<ASTNode> condition_;
        std::shared_ptr<ASTNode> thenBranch_;
        std::shared_ptr<ASTNode> elseBranch_;  // 可能为nullptr

    public:
        IfStatementNode(std::shared_ptr<ASTNode> cond,
            std::shared_ptr<ASTNode> thenBr,
            std::shared_ptr<ASTNode> elseBr = nullptr)
            : ASTNode(ASTNodeType::IF_STATEMENT),
            condition_(cond), thenBranch_(thenBr), elseBranch_(elseBr) {}

        std::shared_ptr<ASTNode> getCondition() const { return condition_; }
        std::shared_ptr<ASTNode> getThenBranch() const { return thenBranch_; }
        std::shared_ptr<ASTNode> getElseBranch() const { return elseBranch_; }
        bool hasElse() const { return elseBranch_ != nullptr; }

        void print(std::ostream& os, int indent = 0) const override;
        std::string getNodeTypeName() const override { return "IfStatement"; }
    };

    // ==================== While语句节点 ====================
    class WhileStatementNode : public ASTNode {
    private:
        std::shared_ptr<ASTNode> condition_;
        std::shared_ptr<ASTNode> body_;

    public:
        WhileStatementNode(std::shared_ptr<ASTNode> cond, std::shared_ptr<ASTNode> body)
            : ASTNode(ASTNodeType::WHILE_STATEMENT), condition_(cond), body_(body) {}

        std::shared_ptr<ASTNode> getCondition() const { return condition_; }
        std::shared_ptr<ASTNode> getBody() const { return body_; }

        void print(std::ostream& os, int indent = 0) const override;
        std::string getNodeTypeName() const override { return "WhileStatement"; }
    };

    // ==================== For语句节点 ====================
    class ForStatementNode : public ASTNode {
    private:
        std::shared_ptr<ASTNode> init_;
        std::shared_ptr<ASTNode> condition_;
        std::shared_ptr<ASTNode> update_;
        std::shared_ptr<ASTNode> body_;

    public:
        ForStatementNode(std::shared_ptr<ASTNode> init,
            std::shared_ptr<ASTNode> cond,
            std::shared_ptr<ASTNode> update,
            std::shared_ptr<ASTNode> body)
            : ASTNode(ASTNodeType::FOR_STATEMENT),
            init_(init), condition_(cond), update_(update), body_(body) {}

        std::shared_ptr<ASTNode> getInit() const { return init_; }
        std::shared_ptr<ASTNode> getCondition() const { return condition_; }
        std::shared_ptr<ASTNode> getUpdate() const { return update_; }
        std::shared_ptr<ASTNode> getBody() const { return body_; }

        void print(std::ostream& os, int indent = 0) const override;
        std::string getNodeTypeName() const override { return "ForStatement"; }
    };

    // ==================== 复合语句节点 ====================
    class CompoundStatementNode : public ASTNode {
    private:
        std::shared_ptr<ASTNode> statements_;

    public:
        CompoundStatementNode(std::shared_ptr<ASTNode> stmts)
            : ASTNode(ASTNodeType::COMPOUND_STATEMENT), statements_(stmts) {}

        std::shared_ptr<ASTNode> getStatements() const { return statements_; }

        void print(std::ostream& os, int indent = 0) const override;
        std::string getNodeTypeName() const override { return "CompoundStatement"; }
    };

    // ==================== 表达式语句节点 ====================
    class ExpressionStatementNode : public ASTNode {
    private:
        std::shared_ptr<ASTNode> expression_;  // 可能为nullptr(空语句 ;)

    public:
        ExpressionStatementNode(std::shared_ptr<ASTNode> expr = nullptr)
            : ASTNode(ASTNodeType::EXPRESSION_STATEMENT), expression_(expr) {}

        std::shared_ptr<ASTNode> getExpression() const { return expression_; }
        bool isEmpty() const { return expression_ == nullptr; }

        void print(std::ostream& os, int indent = 0) const override;
        std::string getNodeTypeName() const override { return "ExpressionStatement"; }
    };

    // ==================== Read语句节点 ====================
    class ReadStatementNode : public ASTNode {
    private:
        std::string varName_;

    public:
        ReadStatementNode(const std::string& name, std::size_t line, std::size_t column)
            : ASTNode(ASTNodeType::READ_STATEMENT, line, column), varName_(name) {}

        std::string getVarName() const { return varName_; }

        void print(std::ostream& os, int indent = 0) const override;
        std::string getNodeTypeName() const override { return "ReadStatement"; }
    };

    // ==================== Write语句节点 ====================
    class WriteStatementNode : public ASTNode {
    private:
        std::shared_ptr<ASTNode> expression_;

    public:
        WriteStatementNode(std::shared_ptr<ASTNode> expr)
            : ASTNode(ASTNodeType::WRITE_STATEMENT), expression_(expr) {}

        std::shared_ptr<ASTNode> getExpression() const { return expression_; }

        void print(std::ostream& os, int indent = 0) const override;
        std::string getNodeTypeName() const override { return "WriteStatement"; }
    };

    // ==================== 二元表达式节点 ====================
    class BinaryExpressionNode : public ASTNode {
    private:
        std::string op_;                    // 运算符
        std::shared_ptr<ASTNode> left_;
        std::shared_ptr<ASTNode> right_;

    public:
        BinaryExpressionNode(const std::string& op,
            std::shared_ptr<ASTNode> left,
            std::shared_ptr<ASTNode> right)
            : ASTNode(ASTNodeType::BINARY_EXPRESSION), op_(op), left_(left), right_(right) {}

        std::string getOperator() const { return op_; }
        std::shared_ptr<ASTNode> getLeft() const { return left_; }
        std::shared_ptr<ASTNode> getRight() const { return right_; }

        void print(std::ostream& os, int indent = 0) const override;
        std::string getNodeTypeName() const override { return "BinaryExpression"; }
    };

    // ==================== 标识符表达式节点 ====================
    class IdentifierNode : public ASTNode {
    private:
        std::string name_;

    public:
        IdentifierNode(const std::string& name, std::size_t line, std::size_t column)
            : ASTNode(ASTNodeType::IDENTIFIER_EXPRESSION, line, column), name_(name) {}

        std::string getName() const { return name_; }

        void print(std::ostream& os, int indent = 0) const override;
        std::string getNodeTypeName() const override { return "Identifier"; }
    };

    // ==================== 数字字面量节点 ====================
    class NumberLiteralNode : public ASTNode {
    private:
        std::string value_;

    public:
        NumberLiteralNode(const std::string& val, std::size_t line, std::size_t column)
            : ASTNode(ASTNodeType::NUMBER_LITERAL, line, column), value_(val) {}

        std::string getValue() const { return value_; }
        int getIntValue() const { return std::stoi(value_); }

        void print(std::ostream& os, int indent = 0) const override;
        std::string getNodeTypeName() const override { return "NumberLiteral"; }
    };

    // ==================== 空节点 ====================
    class EmptyNode : public ASTNode {
    public:
        EmptyNode() : ASTNode(ASTNodeType::EMPTY) {}

        void print(std::ostream& os, int indent = 0) const override;
        std::string getNodeTypeName() const override { return "Empty"; }
    };

    // ==================== 辅助函数 ====================
    // 可视化打印AST
    void printAST(const std::shared_ptr<ASTNode>& root, std::ostream& os = std::cout);

    // 生成缩进字符串
    std::string getIndent(int level);

} // namespace Compiler

#endif // AST_HPP
