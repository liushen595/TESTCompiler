#pragma once

#ifndef SIMULATOR_HPP
#define SIMULATOR_HPP

#include <string>
#include <vector>
#include <stack>
#include <unordered_map>
#include <iostream>
#include <fstream>

#include "Semantic.hpp"

namespace Compiler {

    // 模拟器类 - 用于执行生成的代码
    class Simulator {
    private:
        std::vector<std::string> code_; // 目标机代码
        std::stack<int> operationStack_; // 操作数栈
        std::unordered_map<std::string, int> labels_; // 标签地址映射
        std::unordered_map<int, int> memory_; // 内存地址映射
        int pc_; // 程序计数器

    public:
        Simulator(std::ifstream &filein);
        Simulator(std::vector<std::string> code);

        void LOAD(int address);
        void LOADI(int value);
        void STO(int address);
        void POP();
        void ADD();
        void SUB();
        void MULT();
        void DIV();
        void BR(const std::string &label);
        void BRF(const std::string &label);
        void EQ();
        void NOTEQ();
        void GT();
        void LES();
        void GE();
        void LE();
        void AND();
        void OR();
        void NOT();
        void IN();
        void OUT();
        void STOP();

        void run();
    };

} // namespace Compiler

#endif // SIMULATOR_HPP