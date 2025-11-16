#include "Semantic.hpp"
#include <fstream>

namespace Compiler {

    SemanticAnalyzer::SemanticAnalyzer()
        : labelCounter_(0) {}

    int SemanticAnalyzer::lookupAddress(const std::string& name) const {
        for (int i = static_cast<int>(varOrder_.size()) - 1; i >= 0; --i) {
            if (varOrder_[static_cast<size_t>(i)] == name) return i;
        }
        return -1;
    }

    void SemanticAnalyzer::declareVariable(const std::string& name, std::size_t line, std::size_t column) {
        if (lookupAddress(name) != -1) {
            throw SemanticException("Redefinition of variable '" + name + "'", line, column);
        }
        varOrder_.push_back(name);
    }

    std::string SemanticAnalyzer::newLabel() {
        ++labelCounter_;
        return std::string("LABEL") + std::to_string(labelCounter_);
    }

    void SemanticAnalyzer::emitLabel(const std::string& label) {
        code_.push_back(label + ":");
    }

    void SemanticAnalyzer::emit(const std::string& instr) {
        code_.push_back(instr);
    }

    void SemanticAnalyzer::emitLoad(int address) {
        emit("LOAD " + std::to_string(address));
    }

    void SemanticAnalyzer::emitLoadI(const std::string& value) {
        emit("LOADI " + value);
    }

    void SemanticAnalyzer::emitStore(int address) {
        emit("STO " + std::to_string(address));
    }

    void SemanticAnalyzer::emitPop() {
        emit("POP");
    }

    void SemanticAnalyzer::emitAdd() {
        emit("ADD");
    }

    void SemanticAnalyzer::emitSub() {
        emit("SUB");
    }

    void SemanticAnalyzer::emitMult() {
        emit("MULT");
    }

    void SemanticAnalyzer::emitDiv() {
        emit("DIV");
    }

    void SemanticAnalyzer::emitGT() {
        emit("GT");
    }

    void SemanticAnalyzer::emitLES() {
        emit("LES");
    }

    void SemanticAnalyzer::emitGE() {
        emit("GE");
    }

    void SemanticAnalyzer::emitLE() {
        emit("LE");
    }

    void SemanticAnalyzer::emitEQ() {
        emit("EQ");
    }

    void SemanticAnalyzer::emitNOTEQ() {
        emit("NOTEQ");
    }

    void SemanticAnalyzer::emitBRF(const std::string& label) {
        emit("BRF " + label);
    }

    void SemanticAnalyzer::emitBR(const std::string& label) {
        emit("BR " + label);
    }

    void SemanticAnalyzer::emitIN() {
        emit("IN");
    }

    void SemanticAnalyzer::emitOUT() {
        emit("OUT");
    }

    void SemanticAnalyzer::emitSTOP() {
        emit("STOP");
    }

    void SemanticAnalyzer::reset() {
        varOrder_.clear();
        code_.clear();
        labelCounter_ = 0;
    }

    void SemanticAnalyzer::writeCodeToFile(const std::string& path) const {
        std::ofstream ofs(path);
        for (const auto& line : code_) {
            ofs << line << "\n";
        }
    }

} // namespace Compiler
