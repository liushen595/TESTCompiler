#include "RegexEngine.hpp"
#include "NFA.hpp"
#include <fstream>
#include <sstream>
#include <stack>
#include <iostream>
#include <algorithm>

// 定义操作符的优先级
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

// 定义操作符的结合性（左结合或右结合）
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

void print_nfa(std::shared_ptr<Compiler::NFA> nfa) {
    std::cout << "  NFA Structure:" << std::endl;
    std::cout << "    Total states: " << nfa->getAllStates().size() << std::endl;
    std::cout << "    Start state: " << nfa->getStartState()->getId() << std::endl;
    std::cout << "    Final state: " << nfa->getFinalState()->getId() << std::endl;

    // 打印所有状态和转移
    for (auto state : nfa->getAllStates()) {
        std::cout << "    State " << state->getId()
            << (state->isFinalState() ? " (FINAL)" : "")
            << (!state->getTokenName().empty() ? " [" + state->getTokenName() + "]" : "")
            << ":" << std::endl;

        // 打印非ε转移
        for (auto [symbol, targets] : state->getTransitions()) {
            for (auto target : targets) {
                std::cout << "      --" << symbol << "--> " << target->getId() << std::endl;
            }
        }

        // 打印ε转移
        for (auto target : state->getEpsilonTransitions()) {
            std::cout << "      --ε--> " << target->getId() << std::endl;
        }
    }
    std::cout << "NFA print done. Ready to construct DFA. " << std::endl;
    std::cout << std::endl;
}

namespace Compiler {

    bool RegexEngine::loadRulesFromFile(const std::string& filePath) {
        // 从文件中读取正则表达式规则
        // 格式为：<tokenName>  pattern  priority
        // 每行一个规则

        std::ifstream file(filePath);
        if (!file.is_open()) {
            std::cerr << "Error: Unable to open file " << filePath << std::endl;
            return false;
        }

        std::string line;
        while (std::getline(file, line)) {
            // 跳过空行和注释行
            if (line.empty() || line[0] == '#') {
                continue;
            }

            std::istringstream iss(line);
            std::string tokenName, pattern;
            int priority;

            if (!(iss >> tokenName >> pattern)) {
                std::cerr << "Error: Invalid rule format in line: " << line << std::endl;
                continue; // 跳过格式错误的行
            }

            // 读取优先级(如果存在)
            if (iss >> priority) {
                tokenPriorities[tokenName] = priority;
            }
            else {
                tokenPriorities[tokenName] = 0; // 默认优先级为0
            }

            regexrules[tokenName] = pattern;  // map会自动按key排序

            std::cout << "Loaded rule: " << tokenName
                << " -> " << pattern
                << " (priority: " << tokenPriorities[tokenName] << ")" << std::endl;
        }

        file.close();
        std::cout << "Total rules loaded: " << regexrules.size() << std::endl;
        std::cout << std::endl;
        std::cout << "====================Starting regex to NFA conversion...========================" << std::endl;
        std::cout << std::endl;
        return true;
    }

    const std::map<std::string, std::string>& RegexEngine::getRules() const {
        return regexrules;
    }

    // 使用MYT算法将正则表达式转换为NFA
    void RegexEngine::regexToNFA(std::map<std::string, std::string> &regexrules) {
        // 1. 预处理正则表达式
        preprocessRegex(regexrules);

        // 2. 将中缀表达式转换为后缀表达式并构建每个规则的NFA
        for (auto &item : regexrules) {
            std::string token_name = item.first;
            std::string &regex = item.second;

            // 跳过宏定义(优先级为0的token)
            if (tokenPriorities.count(token_name) && tokenPriorities[token_name] == 0) {
                std::cout << "Skipping macro definition: " << token_name << std::endl;
                continue;
            }

            std::stack<std::shared_ptr<NFA>> nfaStack;
            bool hasError = false;

            std::string postfix = infixToPostfix(regex);
            std::cout << "Processing token " << token_name << std::endl;
            std::cout << "  Original regex: " << regex << std::endl;
            std::cout << "  Postfix: " << postfix << std::endl;

            for (size_t i = 0; i < postfix.size(); i++) {
                char c = postfix[i];
                if (c == '|') {
                    if (nfaStack.size() < 2) {
                        std::cerr << "Error: Invalid regex for union operation in token " << token_name
                            << " (stack size: " << nfaStack.size() << ", position: " << i << ")" << std::endl;
                        hasError = true;
                        break;
                    }
                    std::shared_ptr<NFA> second = nfaStack.top(); nfaStack.pop();
                    std::shared_ptr<NFA> first = nfaStack.top(); nfaStack.pop();
                    nfaStack.push(createUnion(first, second));
                }
                else if (c == '.') {
                    if (nfaStack.size() < 2) {
                        std::cerr << "Error: Invalid regex for concatenation operation in token " << token_name
                            << " (stack size: " << nfaStack.size() << ", position: " << i << ")" << std::endl;
                        hasError = true;
                        break;
                    }
                    std::shared_ptr<NFA> second = nfaStack.top(); nfaStack.pop();
                    std::shared_ptr<NFA> first = nfaStack.top(); nfaStack.pop();
                    nfaStack.push(createConcatenation(first, second));
                }
                else if (c == '*') {
                    if (nfaStack.empty()) {
                        std::cerr << "Error: Invalid regex for Kleene star operation in token " << token_name
                            << " (stack empty, position: " << i << ")" << std::endl;
                        hasError = true;
                        break;
                    }
                    std::shared_ptr<NFA> nfa = nfaStack.top(); nfaStack.pop();
                    nfaStack.push(createKleeneClosure(nfa));
                }
                else if (c == '+') {
                    if (nfaStack.empty()) {
                        std::cerr << "Error: Invalid regex for positive closure operation in token " << token_name
                            << " (stack empty, position: " << i << ")" << std::endl;
                        hasError = true;
                        break;
                    }
                    std::shared_ptr<NFA> nfa = nfaStack.top(); nfaStack.pop();
                    nfaStack.push(createPositiveClosure(nfa));
                }
                else if (c == '\\') {
                    // 处理转义字符
                    if (i + 1 < postfix.size()) {
                        char next_char = postfix[i + 1];
                        nfaStack.push(createBasicNFA(next_char));
                        i++; // 跳过下一个字符
                    }
                    else {
                        std::cerr << "Error: Incomplete escape sequence in token " << token_name << std::endl;
                        hasError = true;
                        break;
                    }
                }
                else {
                    // 处理普通字符
                    nfaStack.push(createBasicNFA(c));
                }
            }

            if (hasError) {
                std::cerr << "Failed to create NFA for token " << token_name << std::endl;
                continue;
            }

            if (nfaStack.size() != 1) {
                std::cerr << "Error: Invalid regex for token " << token_name
                    << " (final stack size: " << nfaStack.size() << ")" << std::endl;
                continue;
            }
            else {
                std::cout << "Successfully created NFA for token " << token_name << std::endl;

                auto final_nfa = nfaStack.top();
                // 调试输出：打印 NFA 结构
                // print_nfa(final_nfa);

                // 设置token名称和优先级
                final_nfa->getFinalState()->setTokenName(token_name);
                int priority = tokenPriorities.count(token_name) ? tokenPriorities[token_name] : 0;
                final_nfa->getFinalState()->setPriority(priority);

                nfa_map[token_name] = final_nfa;
            }
        }
        std::cout << std::endl;
        std::cout << "=====================Finished regex to NFA conversion, ready to combine=======================" << std::endl;
        std::cout << std::endl;
        return;
    }

    std::shared_ptr<NFA> RegexEngine::buildCombinedNFA() {
        regexToNFA(regexrules);
        if (nfa_map.empty()) {
            std::cerr << "Error: No NFA rules loaded." << std::endl;
            return nullptr;
        }
        if (nfa_map.size() == 1) {
            print_nfa(nfa_map.begin()->second);
            return nfa_map.begin()->second;
        }

        std::shared_ptr<NFA> combinedNFA = std::make_shared<NFA>();

        // 创建新的开始状态
        std::shared_ptr<NFAState> startState = combinedNFA->createState(false);
        combinedNFA->setStartState(startState);

        // 新的接受状态
        std::shared_ptr<NFAState> finalState = combinedNFA->createState(true);
        combinedNFA->setFinalState(finalState);

        // 将所有 NFA 的状态和转移复制到合并的 NFA 中
        for (const auto& [tokenName, tokenNFA] : nfa_map) {
            // 复制当前 NFA 的所有状态
            std::map<std::shared_ptr<NFAState>, std::shared_ptr<NFAState>> stateMap;

            // 先复制所有状态
            for (const auto& state : tokenNFA->getAllStates()) {
                std::shared_ptr<NFAState> newState = combinedNFA->createState(false);
                stateMap[state] = newState;
            }

            // 复制所有转移
            for (const auto& state : tokenNFA->getAllStates()) {
                std::shared_ptr<NFAState> newState = stateMap[state];

                // 复制所有非 ε 转移
                for (const auto& [symbol, targets] : state->getTransitions()) {
                    for (const auto& target : targets) {
                        newState->addTransition(symbol, stateMap[target]);
                    }
                }

                // 复制所有 ε 转移
                for (const auto& target : state->getEpsilonTransitions()) {
                    newState->addEpsilonTransition(stateMap[target]);
                }
            }

            // 从新的开始状态添加 ε 转移到当前 NFA 的开始状态
            startState->addEpsilonTransition(stateMap[tokenNFA->getStartState()]);

            // 从当前 NFA 的接受状态添加 ε 转移到新的接受状态，并保留 token 信息和优先级
            std::shared_ptr<NFAState> tokenFinalState = stateMap[tokenNFA->getFinalState()];
            tokenFinalState->addEpsilonTransition(finalState);

            // 保留原小NFA终态的token名称和优先级(这些状态现在变成中间状态)
            tokenFinalState->setTokenName(tokenNFA->getFinalState()->getTokenName());
            tokenFinalState->setPriority(tokenNFA->getFinalState()->getPriority());
        }

        std::cout << "Successfully built combined NFA." << std::endl;
        print_nfa(combinedNFA);
        return combinedNFA;
    }

    // 判断是否为操作符
    bool RegexEngine::isOperator(char c, char c_pre = '\n') const {
        return (c == '|' || c == '*' || c == '+' || c == '.' || c == '(' || c == ')');
    }

    // 预处理正则表达式，处理宏定义和添加显式连接符
    void RegexEngine::preprocessRegex(std::map<std::string, std::string> &regexrules) {
        // 处理双字符操作符
        for (auto &item : regexrules) {
            const std::string &parttern = item.first;
            std::string parttern_name = parttern.substr(1, parttern.size() - 2);
            std::string &regex = item.second;

            for (size_t i = 0; i < regex.size(); i++) {
                if ((regex[i] == '<' && regex[i + 1] == '=') ||
                    (regex[i] == '>' && regex[i + 1] == '=') ||
                    (regex[i] == '!' && regex[i + 1] == '=') ||
                    (regex[i] == '=' && regex[i + 1] == '=')) {
                    regex.insert(i + 1, ".");
                    i++;
                }
            }
        }

        // 第二步：宏展开（多轮，直到没有变化）
        bool changed = true;
        int maxIterations = 10; // 防止无限循环
        int iteration = 0;

        while (changed && iteration < maxIterations) {
            changed = false;
            iteration++;

            for (auto &item : regexrules) {
                const std::string &pattern = item.first;
                std::string &regex = item.second;

                for (size_t i = 0; i < regex.size(); i++) {
                    if (regex[i] == '<' && (i + 1 < regex.size()) && regex[i + 1] != '|' && regex[i + 1] != '=') {
                        // 查找宏定义结束位置
                        size_t macro_end = regex.find('>', i + 1);

                        if (macro_end != std::string::npos) {
                            std::string macro_name = regex.substr(i, macro_end - i + 1);

                            // 查找宏定义
                            auto it = regexrules.find(macro_name);
                            if (it != regexrules.end()) {
                                // 防止自引用
                                if (macro_name != pattern) {
                                    // 用括号包裹宏内容
                                    std::string macro_content = "(" + it->second + ")";
                                    regex.replace(i, macro_end - i + 1, macro_content);
                                    changed = true;

                                    // 更新索引
                                    i += macro_content.size() - 1;
                                }
                            }
                            else {
                                std::string pattern_name = pattern.substr(1, pattern.size() - 2);
                                std::cerr << "Error: Undefined macro " << macro_name
                                    << " in pattern " << pattern_name << std::endl;
                            }
                        }
                    }
                }
            }
        }

        if (iteration >= maxIterations) {
            std::cerr << "Warning: Maximum macro expansion iterations reached. "
                << "Check for circular macro definitions." << std::endl;
        }

        // 第三步：统一添加显式连接符
        for (auto &item : regexrules) {
            std::string &regex = item.second;
            regex = addExplicitConcatenation(regex);

            // 调试输出
            std::string pattern_name = item.first.substr(1, item.first.size() - 2);
            std::cout << "After preprocessing " << pattern_name << ": " << regex << std::endl;
        }
        std::cout << std::endl; // 调试输出

        return;
    }

    // 添加显式连接符
    std::string RegexEngine::addExplicitConcatenation(const std::string& regex) {
        std::string result;

        for (size_t i = 0; i < regex.size(); i++) {
            char current = regex[i];
            result += current;

            // 处理转义字符
            if (current == '\\' && i + 1 < regex.size()) {
                result += regex[i + 1];
                i++; // 跳过被转义的字符
                current = regex[i]; // 更新 current，用于后续连接符判断
            }

            // 如果还有下一个字符，检查是否需要添加连接符
            if (i + 1 < regex.size()) {
                char next = regex[i + 1];

                // 跳过转义的下一个字符判断
                // if (next == '\\' && i + 2 < regex.size()) {
                //     next = regex[i + 1]; // next 实际是转义符
                // }

                // 需要添加连接符的情况：
                // 1. 当前是：字母数字、')'、'*'、'+'、'/'、普通字符
                // 2. 下一个是：字母数字、'('、'\'、普通字符

                bool currentNeedsConcat = (
                    std::isalnum(current) ||
                    current == ')' ||
                    current == '*' ||
                    current == '+' ||
                    current == '/' ||
                    (!isOperator(current) && current != '(')
                    );

                bool nextNeedsConcat = (
                    std::isalnum(next) ||
                    next == '(' ||
                    next == '\\' || // 转义符，有转义说明是普通字符，那么就需要连接符
                    (!isOperator(next) && next != ')')
                    );

                if (currentNeedsConcat && nextNeedsConcat) {
                    result += '.';
                }
            }
        }

        return result;
    }

    // 将中缀表达式转换为后缀表达式
    std::string RegexEngine::infixToPostfix(const std::string& regex) {
        std::string postfix; // 存储后缀表达式
        std::stack<char> ops; // 操作符栈
        if (regex == "/.\\*" || regex == "\\*./") {
            if (regex == "/.\\*") return "/\\*.";
            else return "\\*/."; // 如果是注释符号，直接写成后缀并返回
        }

        for (size_t i = 0; i < regex.size(); i++) {
            char c = regex[i];
            if (std::isalnum(c)) {
                postfix += c;
            }
            else if (c == '\\') { // 转义字符
                if (i + 1 < regex.size()) {
                    postfix += c;
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
                while (!ops.empty() && ops.top() != '(' &&
                    (precedence(ops.top()) > precedence(c) || (precedence(ops.top()) == precedence(c) && isLeftAssociative(c)))) {
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

    // 创建基本NFA，接受单个字符
    std::shared_ptr<NFA> RegexEngine::createBasicNFA(char c) {
        std::shared_ptr<NFA> nfa = std::make_shared<NFA>();

        std::shared_ptr<NFAState> start = nfa->createState(false);
        std::shared_ptr<NFAState> accept = nfa->createState(true);

        start->addTransition(c, accept);
        nfa->setStartState(start);
        nfa->setFinalState(accept);

        return nfa;
    }

    // 创建两个NFA的连接
    std::shared_ptr<NFA> RegexEngine::createConcatenation(std::shared_ptr<NFA> first, std::shared_ptr<NFA> second) {
        // 创建新的nfa
        std::shared_ptr<NFA> nfa = std::make_shared<NFA>();

        // 复制第一个NFA的所有状态
        std::map<std::shared_ptr<NFAState>, std::shared_ptr<NFAState>> first_state_map;
        for (const auto state : first->getAllStates()) {
            auto new_state = nfa->createState(false); // 第一个NFA的所有状态都不是终结状态
            first_state_map[state] = new_state;
        }

        // 复制第一个NFA的所有转移
        for (auto old_state : first->getAllStates()) {
            auto new_state = first_state_map[old_state];

            // 非ε转移
            for (auto transiton : old_state->getTransitions()) {
                char symbol = transiton.first;
                for (auto target : transiton.second) {
                    new_state->addTransition(symbol, first_state_map[target]);
                }
            }

            // ε转移
            for (auto target : old_state->getEpsilonTransitions()) {
                new_state->addEpsilonTransition(first_state_map[target]);
            }
        }

        // 复制第二个NFA的所有状态
        std::map<std::shared_ptr<NFAState>, std::shared_ptr<NFAState>> second_state_map;
        for (const auto state : second->getAllStates()) {
            auto new_state = nfa->createState(state->isFinalState()); // 保持第二个NFA的终结状态属性
            second_state_map[state] = new_state;
        }

        // 复制第二个NFA的所有转移
        for (auto old_state : second->getAllStates()) {
            auto new_state = second_state_map[old_state];

            // 非ε转移
            for (auto transiton : old_state->getTransitions()) {
                char symbol = transiton.first;
                for (auto target : transiton.second) {
                    new_state->addTransition(symbol, second_state_map[target]);
                }
            }

            // ε转移
            for (auto target : old_state->getEpsilonTransitions()) {
                new_state->addEpsilonTransition(second_state_map[target]);
            }
        }

        // 连接两个NFA：将第一个NFA的终结状态通过ε转移连接到第二个NFA的开始状态
        first_state_map[first->getFinalState()]->addEpsilonTransition(second_state_map[second->getStartState()]);

        // 设置新NFA的开始和终结状态
        nfa->setStartState(first_state_map[first->getStartState()]);
        nfa->setFinalState(second_state_map[second->getFinalState()]);

        return nfa;
    }

    // 创建两个NFA的选择
    std::shared_ptr<NFA> RegexEngine::createUnion(std::shared_ptr<NFA> first, std::shared_ptr<NFA> second) {
        std::shared_ptr<NFA> nfa = std::make_shared<NFA>();

        // 创建新的开始和接受状态
        std::shared_ptr<NFAState> new_start = nfa->createState(false);
        std::shared_ptr<NFAState> new_accept = nfa->createState(true);

        // 复制第一个NFA的所有状态
        std::map<std::shared_ptr<NFAState>, std::shared_ptr<NFAState>> first_state_map;
        for (const auto state : first->getAllStates()) {
            auto new_state = nfa->createState(false); // 原来的终结状态不再是终结状态
            first_state_map[state] = new_state;
        }

        // 复制第一个NFA的所有转移
        for (auto old_state : first->getAllStates()) {
            auto new_state = first_state_map[old_state];

            // 非ε转移
            for (auto transiton : old_state->getTransitions()) {
                char symbol = transiton.first;
                for (auto target : transiton.second) {
                    new_state->addTransition(symbol, first_state_map[target]);
                }
            }

            // ε转移
            for (auto target : old_state->getEpsilonTransitions()) {
                new_state->addEpsilonTransition(first_state_map[target]);
            }
        }

        // 复制第二个NFA的所有状态
        std::map<std::shared_ptr<NFAState>, std::shared_ptr<NFAState>> second_state_map;
        for (const auto state : second->getAllStates()) {
            auto new_state = nfa->createState(false); // 原来的终结状态不再是终结状态
            second_state_map[state] = new_state;
        }

        // 复制第二个NFA的所有转移
        for (auto old_state : second->getAllStates()) {
            auto new_state = second_state_map[old_state];

            // 非ε转移
            for (auto transiton : old_state->getTransitions()) {
                char symbol = transiton.first;
                for (auto target : transiton.second) {
                    new_state->addTransition(symbol, second_state_map[target]);
                }
            }

            // ε转移
            for (auto target : old_state->getEpsilonTransitions()) {
                new_state->addEpsilonTransition(second_state_map[target]);
            }
        }

        // 新的开始状态通过ε转移连接到两个NFA的开始状态
        new_start->addEpsilonTransition(first_state_map[first->getStartState()]);
        new_start->addEpsilonTransition(second_state_map[second->getStartState()]);

        // 两个NFA的终结状态通过ε转移连接到新的终结状态
        first_state_map[first->getFinalState()]->addEpsilonTransition(new_accept);
        second_state_map[second->getFinalState()]->addEpsilonTransition(new_accept);

        // 设置新NFA的开始和终结状态
        nfa->setStartState(new_start);
        nfa->setFinalState(new_accept);

        return nfa;
    }

    // 创建NFA的Kleene闭包
    std::shared_ptr<NFA> RegexEngine::createKleeneClosure(std::shared_ptr<NFA> nfa) {
        std::shared_ptr<NFA> result = std::make_shared<NFA>();

        // 创建新的开始和接受状态
        std::shared_ptr<NFAState> new_start = result->createState(false);
        std::shared_ptr<NFAState> new_accept = result->createState(true);

        // 复制原NFA的所有状态（所有状态都不是终结状态）
        std::map<std::shared_ptr<NFAState>, std::shared_ptr<NFAState>> state_map;
        for (const auto state : nfa->getAllStates()) {
            auto new_state = result->createState(false); // 所有复制的状态都不是终结状态
            state_map[state] = new_state;
        }

        // 复制原NFA的所有转移
        for (auto old_state : nfa->getAllStates()) {
            auto new_state = state_map[old_state];

            // 非ε转移
            for (auto transiton : old_state->getTransitions()) {
                char symbol = transiton.first;
                for (auto target : transiton.second) {
                    new_state->addTransition(symbol, state_map[target]);
                }
            }

            // ε转移
            for (auto target : old_state->getEpsilonTransitions()) {
                new_state->addEpsilonTransition(state_map[target]);
            }
        }

        // 新的开始状态通过ε转移连接到原NFA的开始状态和新的接受状态
        new_start->addEpsilonTransition(state_map[nfa->getStartState()]);
        new_start->addEpsilonTransition(new_accept);

        // 原NFA的终结状态通过ε转移连接到原NFA的开始状态和新的接受状态
        state_map[nfa->getFinalState()]->addEpsilonTransition(state_map[nfa->getStartState()]);
        state_map[nfa->getFinalState()]->addEpsilonTransition(new_accept);

        result->setStartState(new_start);
        result->setFinalState(new_accept);

        return result;
    }

    // 创建NFA的正闭包
    std::shared_ptr<NFA> RegexEngine::createPositiveClosure(std::shared_ptr<NFA> nfa) {
        std::shared_ptr<NFA> result = std::make_shared<NFA>();

        // 创建新的开始和接受状态
        std::shared_ptr<NFAState> new_start = result->createState(false);
        std::shared_ptr<NFAState> new_accept = result->createState(true);

        // 复制原NFA的所有状态（所有状态都不是终结状态）
        std::map<std::shared_ptr<NFAState>, std::shared_ptr<NFAState>> state_map;
        for (const auto state : nfa->getAllStates()) {
            auto new_state = result->createState(false); // 所有复制的状态都不是终结状态
            state_map[state] = new_state;
        }

        // 复制原NFA的所有转移
        for (auto old_state : nfa->getAllStates()) {
            auto new_state = state_map[old_state];

            // 非ε转移
            for (auto transiton : old_state->getTransitions()) {
                char symbol = transiton.first;
                for (auto target : transiton.second) {
                    new_state->addTransition(symbol, state_map[target]);
                }
            }

            // ε转移
            for (auto target : old_state->getEpsilonTransitions()) {
                new_state->addEpsilonTransition(state_map[target]);
            }
        }

        // 新的开始状态通过ε转移连接到原NFA的开始状态
        new_start->addEpsilonTransition(state_map[nfa->getStartState()]);

        // 原NFA的终结状态通过ε转移连接到原NFA的开始状态（实现循环重复）
        state_map[nfa->getFinalState()]->addEpsilonTransition(state_map[nfa->getStartState()]);

        // 原NFA的终结状态通过ε转移连接到新的接受状态
        state_map[nfa->getFinalState()]->addEpsilonTransition(new_accept);

        result->setStartState(new_start);
        result->setFinalState(new_accept);

        return result;
    }

    // std::shared_ptr<NFA> RegexEngine::createOptional(std::shared_ptr<NFA> nfa) {
    //     // 创建NFA的可选项

    //     // TODO: 实现NFA的可选项逻辑

    //     return nullptr; // 暂时返回空指针，后续需要实现
    // }

} // namespace Compiler
