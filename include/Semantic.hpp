#pragma once

#ifndef SEMANTIC_HPP
#define SEMANTIC_HPP

#include <string>
#include <vector>

namespace Compiler {

    // 语义分析错误类
    class SemanticException : public std::exception {
    private:
        std::string message_;
        std::size_t line_;
        std::size_t column_;

    public:
        SemanticException(const std::string& message, std::size_t line, std::size_t column)
            : message_(message), line_(line), column_(column) {}

        const char* what() const noexcept override {
            return message_.c_str();
        }

        std::size_t getLine() const { return line_; }
        std::size_t getColumn() const { return column_; }

        std::string getFullMessage() const {
            return "SemanticError (in line:" + std::to_string(line_) +
                ", in column:" + std::to_string(column_) + "): " + message_;
        }
    };

    // 语义分析器类 - 符号表管理与代码生成
    class SemanticAnalyzer {
    private:
        std::vector<std::string> varOrder_; // 变量名顺序（地址即下标）
        std::vector<std::string> code_;      // 目标机代码
        int labelCounter_;                   // 标号计数器

    public:
        SemanticAnalyzer();

        // 符号表操作
        int lookupAddress(const std::string& name) const; // 返回 -1 表示未定义
        void declareVariable(const std::string& name, std::size_t line, std::size_t column);

        // 代码生成：标签
        std::string newLabel();
        void emitLabel(const std::string& label);

        // 代码生成：指令
        void emit(const std::string& instr);
        void emitLoad(int address);
        void emitLoadI(const std::string& value);
        void emitStore(int address);
        void emitPop();
        void emitAdd();
        void emitSub();
        void emitMult();
        void emitDiv();
        void emitGT();
        void emitLES();
        void emitGE();
        void emitLE();
        void emitEQ();
        void emitNOTEQ();
        void emitBRF(const std::string& label);
        void emitBR(const std::string& label);
        void emitIN();
        void emitOUT();
        void emitSTOP();

        // 重置状态
        void reset();

        // 获取生成的代码
        const std::vector<std::string>& getCode() const { return code_; }

        // 导出代码到文件
        void writeCodeToFile(const std::string& path) const;
    };

} // namespace Compiler

#endif // SEMANTIC_HPP
