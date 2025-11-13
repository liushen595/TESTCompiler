#include "AST.hpp"
#include <iomanip>

namespace Compiler {

    // ==================== 辅助函数实现 ====================
    std::string getIndent(int level) {
        std::string indent;
        for (int i = 0; i < level; ++i) {
            indent += "│   ";
        }
        return indent;
    }

    void printAST(const std::shared_ptr<ASTNode>& root, std::ostream& os) {
        os << "\n";
        os << "╔════════════════════════════════════════════════════════════╗\n";
        os << "║                    Abstract Syntax Tree                    ║\n";
        os << "╚════════════════════════════════════════════════════════════╝\n";
        if (root) {
            root->print(os, 0);
        }
        else {
            os << "(empty)\n";
        }
        os << "\n";
    }

    // ==================== ProgramNode ====================
    void ProgramNode::print(std::ostream& os, int indent) const {
        os << getIndent(indent) << "├─ Program\n";

        if (declarations_) {
            os << getIndent(indent) << "│   ├─ Declarations:\n";
            declarations_->print(os, indent + 2);
        }

        if (statements_) {
            os << getIndent(indent) << "│   └─ Statements:\n";
            statements_->print(os, indent + 2);
        }
    }

    // ==================== DeclarationListNode ====================
    void DeclarationListNode::print(std::ostream& os, int indent) const {
        if (declarations_.empty()) {
            os << getIndent(indent) << "│   (no declarations)\n";
            return;
        }

        for (size_t i = 0; i < declarations_.size(); ++i) {
            bool isLast = (i == declarations_.size() - 1);
            os << getIndent(indent) << (isLast ? "└─ " : "├─ ");
            declarations_[i]->print(os, indent + (isLast ? 1 : 1));
        }
    }

    // ==================== DeclarationNode ====================
    void DeclarationNode::print(std::ostream& os, int indent) const {
        os << "Declaration: " << varType_ << " " << varName_
            << " (line " << line_ << ", col " << column_ << ")\n";
    }

    // ==================== StatementListNode ====================
    void StatementListNode::print(std::ostream& os, int indent) const {
        if (statements_.empty()) {
            os << getIndent(indent) << "│   (no statements)\n";
            return;
        }

        for (size_t i = 0; i < statements_.size(); ++i) {
            bool isLast = (i == statements_.size() - 1);
            statements_[i]->print(os, indent);
        }
    }

    // ==================== IfStatementNode ====================
    void IfStatementNode::print(std::ostream& os, int indent) const {
        os << getIndent(indent) << "├─ IfStatement\n";

        os << getIndent(indent + 1) << "├─ Condition:\n";
        if (condition_) {
            condition_->print(os, indent + 2);
        }

        os << getIndent(indent + 1) << "├─ Then:\n";
        if (thenBranch_) {
            thenBranch_->print(os, indent + 2);
        }

        if (elseBranch_) {
            os << getIndent(indent + 1) << "└─ Else:\n";
            elseBranch_->print(os, indent + 2);
        }
    }

    // ==================== WhileStatementNode ====================
    void WhileStatementNode::print(std::ostream& os, int indent) const {
        os << getIndent(indent) << "├─ WhileStatement\n";

        os << getIndent(indent + 1) << "├─ Condition:\n";
        if (condition_) {
            condition_->print(os, indent + 2);
        }

        os << getIndent(indent + 1) << "└─ Body:\n";
        if (body_) {
            body_->print(os, indent + 2);
        }
    }

    // ==================== ForStatementNode ====================
    void ForStatementNode::print(std::ostream& os, int indent) const {
        os << getIndent(indent) << "├─ ForStatement\n";

        os << getIndent(indent + 1) << "├─ Init:\n";
        if (init_) {
            init_->print(os, indent + 2);
        }

        os << getIndent(indent + 1) << "├─ Condition:\n";
        if (condition_) {
            condition_->print(os, indent + 2);
        }

        os << getIndent(indent + 1) << "├─ Update:\n";
        if (update_) {
            update_->print(os, indent + 2);
        }

        os << getIndent(indent + 1) << "└─ Body:\n";
        if (body_) {
            body_->print(os, indent + 2);
        }
    }

    // ==================== CompoundStatementNode ====================
    void CompoundStatementNode::print(std::ostream& os, int indent) const {
        os << getIndent(indent) << "├─ CompoundStatement\n";
        if (statements_) {
            statements_->print(os, indent + 1);
        }
    }

    // ==================== ExpressionStatementNode ====================
    void ExpressionStatementNode::print(std::ostream& os, int indent) const {
        os << getIndent(indent) << "├─ ExpressionStatement\n";
        if (expression_) {
            expression_->print(os, indent + 1);
        }
        else {
            os << getIndent(indent + 1) << "└─ (empty statement)\n";
        }
    }

    // ==================== ReadStatementNode ====================
    void ReadStatementNode::print(std::ostream& os, int indent) const {
        os << getIndent(indent) << "├─ ReadStatement: " << varName_
            << " (line " << line_ << ", col " << column_ << ")\n";
    }

    // ==================== WriteStatementNode ====================
    void WriteStatementNode::print(std::ostream& os, int indent) const {
        os << getIndent(indent) << "├─ WriteStatement\n";
        if (expression_) {
            expression_->print(os, indent + 1);
        }
    }

    // ==================== BinaryExpressionNode ====================
    void BinaryExpressionNode::print(std::ostream& os, int indent) const {
        os << getIndent(indent) << "├─ BinaryExpression: " << op_ << "\n";

        os << getIndent(indent + 1) << "├─ Left:\n";
        if (left_) {
            left_->print(os, indent + 2);
        }

        os << getIndent(indent + 1) << "└─ Right:\n";
        if (right_) {
            right_->print(os, indent + 2);
        }
    }

    // ==================== IdentifierNode ====================
    void IdentifierNode::print(std::ostream& os, int indent) const {
        os << getIndent(indent) << "└─ Identifier: " << name_
            << " (line " << line_ << ", col " << column_ << ")\n";
    }

    // ==================== NumberLiteralNode ====================
    void NumberLiteralNode::print(std::ostream& os, int indent) const {
        os << getIndent(indent) << "└─ Number: " << value_
            << " (line " << line_ << ", col " << column_ << ")\n";
    }

    // ==================== EmptyNode ====================
    void EmptyNode::print(std::ostream& os, int indent) const {
        os << getIndent(indent) << "└─ (ε)\n";
    }

} // namespace Compiler
