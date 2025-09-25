#include "RegexEngine.hpp"
#include "NFA.hpp"
#include <fstream>
#include <sstream>
#include <stack>
#include <iostream>
#include <algorithm>

int precedence(char op) {
    switch (op) {
    case '*':
    case '+':
        return 3;
    case '.':
        return 2;
    case '|':
        return 1;
    default:
        return 0;
    }
}

bool isLeftAssociative(char op) {
    switch (op) {
    case '*':
    case '+':
        return false; // 右结合
    case '.':
    case '|':
        return true;  // 左结合
    default:
        return true;  // 默认左结合
    }
}

namespace Compiler {

    bool RegexEngine::loadRulesFromFile(const std::string& filePath) {
        // 从文件中读取正则表达式规则
        // 格式为：正则表达式 TOKEN_NAME 优先级(无)
        // 每行一个规则
        std::ifstream file(filePath);
        if (!file.is_open()) {
            std::cerr << "Error: Unable to open file " << filePath << std::endl;
            return false;
        }
        else {
            std::string line;
            while (std::getline(file, line)) {
                std::istringstream iss(line);
                std::string pattern, tokenName;
                // int priority;
                if (!(iss >> tokenName >> pattern)) {
                    std::cerr << "Error: Invalid rule format in line: " << line << std::endl;
                    continue; // 跳过格式错误的行
                }
                // if (iss >> priority) {
                //     rules.emplace_back(pattern, tokenName, priority);
                // } else {
                //     rules.emplace_back(pattern, tokenName, 0); // 默认优先级为0
                // }
                regexrules[tokenName] = pattern;
            }
            file.close();
        }
        return true;
    }

    const std::unordered_map<std::string, std::string>& RegexEngine::getRules() const {
        return regexrules;
    }

    std::shared_ptr<NFA> RegexEngine::regexToNFA(std::unordered_map<std::string, std::string> &regexrules) {
        // 使用MYT算法将正则表达式转换为NFA

        // 1. 预处理正则表达式
        preprocessRegex(regexrules);

        // 2. 将中缀表达式转换为后缀表达式
        for (auto &item : regexrules) {
            std::string &regex = item.second;
            std::string postfixRegex = infixToPostfix(regex);
            regex = postfixRegex;
        }

        // 3. 构建NFA
        // TODO: 实现使用MYT算法构建NFA的逻辑


        return nullptr; // 暂时返回空指针，后续需要实现
    }

    std::shared_ptr<NFA> RegexEngine::buildCombinedNFA() {
        NFA *combinedNFA = new NFA();
        combinedNFA = regexToNFA(regexrules).get();
        // 将所有规则合并成一个大的NFA

        // TODO: 实现合并所有规则NFA的逻辑

        return nullptr; // 暂时返回空指针，后续需要实现
    }

    void RegexEngine::preprocessRegex(std::unordered_map<std::string, std::string> &regexrules) {
        for (auto &item : regexrules) {
            const std::string &parttern = item.first;
            std::string parttern_name = parttern.substr(1, parttern.size() - 2);
            std::string &regex = item.second;

            for (size_t i = 0; i < regex.size(); i++) {
                size_t macro_begin = regex.find('<', i); // 查找宏定义开始位置

                // 如果包含宏定义, 则进行替换
                if ((macro_begin != std::string::npos) && (regex[macro_begin + 1] != '|' && regex[macro_begin + 1] != '=')) {
                    size_t macro_end = regex.find('>', macro_begin);
                    i = macro_end;

                    if (macro_end != std::string::npos) {
                        regex.replace(macro_begin, 1, "(");
                        regex.replace(macro_end, 1, ")");
                        std::string macro = "<" + regex.substr(macro_begin + 1, macro_end - macro_begin - 1) + ">";
                        auto it = regexrules.find(macro);// 查找宏定义

                        if (it != regexrules.end()) {
                            // 宏替换, 并将宏名称转换为大写, 当作占位符
                            // std::string macro_upper = macro.substr(1, macro.size() - 2);
                            // for (size_t j = 0; j < macro_upper.size(); j++) {
                            //     macro_upper[j] = toupper(macro_upper[j]);
                            // }
                            // 将宏定义单独拿出来
                            // macros[macro_upper] = it->second;
                            // 宏定义本身也要替换成占位符
                            // it->second = macro_upper;
                            regex.replace(macro_begin + 1, macro_end - macro_begin - 1, it->second);
                            i = macro_begin + it->second.size() + 1; // 更新 i 位置，避免重复扫描
                        }
                        else {
                            std::cerr << "Error: Undefined macro <" << macro << "> in pattern " << parttern_name << std::endl;
                        }

                        if (i + 1 < regex.size()) {
                            if (regex[i + 1] == '(' || regex[i + 1] == '<' || std::isalpha(regex[i]) || std::isdigit(regex[i])) {
                                regex.insert(i + 1, ".");
                                i++;
                            }
                        }
                    }
                }
            }
        }
        return;
    }

    std::string RegexEngine::infixToPostfix(const std::string& regex) {
        // 将中缀表达式转换为后缀表达式
        std::string postfix; // 存储后缀表达式
        std::stack<char> ops; // 操作符栈
        if (regex == "/\\*" || regex == "\\*/") {
            if (regex == "/\\*") return "/*";
            else return "*/"; // 如果是注释符号，直接返回
        }

        for (size_t i = 0; i < regex.size(); i++) {
            char c = regex[i];
            if (std::isalnum(c)) {
                postfix += c;
            }
            else if (c == '\\') { // 转义字符
                if (i + 1 < regex.size()) {
                    postfix += regex[i + 1];
                    i++; // 跳过下一个字符
                }
            }
            else if (c == '(') {
                ops.push(c);
            }
            else if (c == ')') {
                while (!ops.empty() && ops.top() != '(') {
                    postfix += ops.top();
                    ops.pop();
                }// stack:(abcde -> abcde(
                if (!ops.empty()) ops.pop(); // 弹出并丢弃左括号
            }
            else if (c == '.' || c == '|' || c == '*' || c == '+') {
                while (!ops.empty() && precedence(ops.top()) >= precedence(c) && isLeftAssociative(c)) {
                    postfix += ops.top();
                    ops.pop();
                }
                ops.push(c);
            }
            else { // 其他符号直接加入后缀表达式
                if (c == '/') {
                    if (i + 1 >= regex.size()) { // 如果是除号
                        postfix += c;
                    }
                }
                else { postfix += c; }
            }
        }
        // 将栈中剩余操作符弹出
        while (!ops.empty()) {
            postfix += ops.top();
            ops.pop();
        }

        return postfix;
    }

    std::shared_ptr<NFA> RegexEngine::createBasicNFA(char c) {
        // 创建基本NFA，接受单个字符
        std::shared_ptr<NFA> nfa = std::make_shared<NFA>();

        NFAState *start = nfa->createState();

        // TODO: 实现创建基本NFA的逻辑

        return nullptr; // 暂时返回空指针，后续需要实现
    }

    std::shared_ptr<NFA> RegexEngine::createConcatenation(std::shared_ptr<NFA> first, std::shared_ptr<NFA> second) {
        // 创建两个NFA的连接

        // TODO: 实现NFA连接的逻辑

        return nullptr; // 暂时返回空指针，后续需要实现
    }

    std::shared_ptr<NFA> RegexEngine::createUnion(std::shared_ptr<NFA> first, std::shared_ptr<NFA> second) {
        // 创建两个NFA的并集

        // TODO: 实现NFA并集的逻辑

        return nullptr; // 暂时返回空指针，后续需要实现
    }

    std::shared_ptr<NFA> RegexEngine::createKleeneClosure(std::shared_ptr<NFA> nfa) {
        // 创建NFA的Kleene闭包

        // TODO: 实现NFA的Kleene闭包逻辑

        return nullptr; // 暂时返回空指针，后续需要实现
    }

    std::shared_ptr<NFA> RegexEngine::createPositiveClosure(std::shared_ptr<NFA> nfa) {
        // 创建NFA的正闭包

        // TODO: 实现NFA的正闭包逻辑

        return nullptr; // 暂时返回空指针，后续需要实现
    }

    std::shared_ptr<NFA> RegexEngine::createOptional(std::shared_ptr<NFA> nfa) {
        // 创建NFA的可选项

        // TODO: 实现NFA的可选项逻辑

        return nullptr; // 暂时返回空指针，后续需要实现
    }

} // namespace Compiler
