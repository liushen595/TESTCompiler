#include "../../include/Simulator.hpp"

#include <sstream>

using std::cout;
using std::cin;
using std::endl;
using std::stack;
using std::string;
using std::vector;
using std::unordered_map;
using std::ifstream;
using std::size_t;
using std::stringstream;


namespace Compiler {

    Simulator::Simulator(ifstream &filein)
        :pc_(0), code_(), operationStack_(), labels_(), memory_() {
        string line;
        while (std::getline(filein, line)) {
            if (!line.empty() && line.back() == '\n') {
                line.pop_back(); // 移除可能的回车符
            }
            code_.push_back(line);
        }

        // 预处理标签地址映射
        for (size_t i = 0; i < code_.size(); ++i) {
            const string &instr = code_[i];
            if (instr.back() == ':') {
                string label = instr.substr(0, instr.size() - 1);
                labels_[label] = static_cast<int>(i);
            }
        }
    }

    Simulator::Simulator(vector<string> code) :
        pc_(0), code_(code), operationStack_(), labels_(), memory_() {
        // 预处理标签地址映射
        for (size_t i = 0; i < code_.size(); ++i) {
            const string &instr = code_[i];
            if (instr.back() == ':') {
                string label = instr.substr(0, instr.size() - 1);
                labels_[label] = static_cast<int>(i);
            }
        }
    }

    void Simulator::LOAD(int address) {
        operationStack_.push(memory_[address]);
    }

    void Simulator::LOADI(int value) {
        operationStack_.push(value);
    }

    void Simulator::STO(int address) {
        int value = operationStack_.top();
        memory_[address] = value;
    }

    void Simulator::POP() {
        operationStack_.pop();
    }

    void Simulator::ADD() {
        int b = operationStack_.top(); operationStack_.pop();
        int a = operationStack_.top(); operationStack_.pop();
        operationStack_.push(a + b);
    }

    void Simulator::SUB() {
        int b = operationStack_.top(); operationStack_.pop();
        int a = operationStack_.top(); operationStack_.pop();
        operationStack_.push(a - b);
    }

    void Simulator::MULT() {
        int b = operationStack_.top(); operationStack_.pop();
        int a = operationStack_.top(); operationStack_.pop();
        operationStack_.push(a * b);
    }

    void Simulator::DIV() {
        int b = operationStack_.top(); operationStack_.pop();
        int a = operationStack_.top(); operationStack_.pop();
        operationStack_.push(a / b);
    }

    void Simulator::BR(const string &label) {
        pc_ = labels_[label];
    }

    void Simulator::BRF(const string &label) {
        int condition = operationStack_.top(); operationStack_.pop();
        if (condition == 0) {
            pc_ = labels_[label];
        }
        else {
            pc_++;
        }
    }

    void Simulator::EQ() {
        int b = operationStack_.top(); operationStack_.pop();
        int a = operationStack_.top(); operationStack_.pop();
        operationStack_.push(a == b ? 1 : 0);
    }

    void Simulator::NOTEQ() {
        int b = operationStack_.top(); operationStack_.pop();
        int a = operationStack_.top(); operationStack_.pop();
        operationStack_.push(a != b ? 1 : 0);
    }

    void Simulator::GT() {
        int b = operationStack_.top(); operationStack_.pop();
        int a = operationStack_.top(); operationStack_.pop();
        operationStack_.push(a > b ? 1 : 0);
    }

    void Simulator::LES() {
        int b = operationStack_.top(); operationStack_.pop();
        int a = operationStack_.top(); operationStack_.pop();
        operationStack_.push(a < b ? 1 : 0);
    }

    void Simulator::GE() {
        int b = operationStack_.top(); operationStack_.pop();
        int a = operationStack_.top(); operationStack_.pop();
        operationStack_.push(a >= b ? 1 : 0);
    }

    void Simulator::LE() {
        int b = operationStack_.top(); operationStack_.pop();
        int a = operationStack_.top(); operationStack_.pop();
        operationStack_.push(a <= b ? 1 : 0);
    }

    void Simulator::AND() {
        int b = operationStack_.top(); operationStack_.pop();
        int a = operationStack_.top(); operationStack_.pop();
        operationStack_.push((a != 0 && b != 0) ? 1 : 0);
    }

    void Simulator::OR() {
        int b = operationStack_.top(); operationStack_.pop();
        int a = operationStack_.top(); operationStack_.pop();
        operationStack_.push((a != 0 || b != 0) ? 1 : 0);
    }

    void Simulator::NOT() {
        int a = operationStack_.top(); operationStack_.pop();
        operationStack_.push((a == 0) ? 1 : 0);
    }

    void Simulator::IN() {
        int value;
        cin >> value;
        operationStack_.push(value);
    }

    void Simulator::OUT() {
        int value = operationStack_.top(); operationStack_.pop();
        cout << value;
    }

    void Simulator::STOP() {
        pc_ = static_cast<int>(code_.size()); // 终止执行
    }

    void Simulator::run() {
        string Cmd, Operation;
        stringstream ss;
        while (pc_ < static_cast<int>(code_.size())) {
            ss.clear();
            ss.str(code_[pc_]);
            ss >> Cmd;

            if (Cmd.back() == ':') {
                // 标签行，跳过
                pc_++;
                continue;
            }
            else if (Cmd == "LOAD") {
                int address;
                ss >> Operation;
                address = std::stoi(Operation);
                LOAD(address);
                pc_++;
                continue;
            }
            else if (Cmd == "LOADI") {
                int value;
                ss >> Operation;
                value = std::stoi(Operation);
                LOADI(value);
                pc_++;
                continue;
            }
            else if (Cmd == "STO") {
                int address;
                ss >> Operation;
                address = std::stoi(Operation);
                STO(address);
                pc_++;
                continue;
            }
            else if (Cmd == "POP") {
                POP();
                pc_++;
                continue;
            }
            else if (Cmd == "ADD") {
                ADD();
                pc_++;
                continue;
            }
            else if (Cmd == "SUB") {
                SUB();
                pc_++;
                continue;
            }
            else if (Cmd == "MULT") {
                MULT();
                pc_++;
                continue;
            }
            else if (Cmd == "DIV") {
                DIV();
                pc_++;
                continue;
            }
            else if (Cmd == "BR") {
                ss >> Operation;
                BR(Operation);
                continue;
            }
            else if (Cmd == "BRF") {
                ss >> Operation;
                BRF(Operation);
                continue;
            }
            else if (Cmd == "EQ") {
                EQ();
                pc_++;
                continue;
            }
            else if (Cmd == "NOTEQ") {
                NOTEQ();
                pc_++;
                continue;
            }
            else if (Cmd == "GT") {
                GT();
                pc_++;
                continue;
            }
            else if (Cmd == "LES") {
                LES();
                pc_++;
                continue;
            }
            else if (Cmd == "GE") {
                GE();
                pc_++;
                continue;
            }
            else if (Cmd == "LE") {
                LE();
                pc_++;
                continue;
            }
            else if (Cmd == "AND") {
                AND();
                pc_++;
                continue;
            }
            else if (Cmd == "OR") {
                OR();
                pc_++;
                continue;
            }
            else if (Cmd == "NOT") {
                NOT();
                pc_++;
                continue;
            }
            else if (Cmd == "IN") {
                IN();
                pc_++;
                continue;
            }
            else if (Cmd == "OUT") {
                OUT();
                pc_++;
                continue;
            }
            else if (Cmd == "STOP") {
                STOP();
                continue;
            }
            else {
                std::cerr << "Unknown instruction: " << Cmd << endl;
                break;
            }
        }
        return;
    }
}