#include "NFA.hpp"
#include "DFA.hpp"
#include <queue>
#include <algorithm>
#include <iostream>

namespace Compiler {

    // NFAState实现
    NFAState::NFAState(size_t id, bool isFinal) : id(id), finalState(isFinal), priority(0) {}

    size_t NFAState::getId() const {
        return id;
    }

    bool NFAState::isFinalState() const {
        return finalState;
    }

    void NFAState::setFinal(bool final) {
        finalState = final;
    }

    void NFAState::addTransition(char symbol, std::shared_ptr<NFAState> target) {
        transitions[symbol].push_back(target);
    }

    void NFAState::addEpsilonTransition(std::shared_ptr<NFAState> target) {
        epsilonTransitions.push_back(target);
    }

    const std::map<char, std::vector<std::shared_ptr<NFAState>>>& NFAState::getTransitions() const {
        return transitions;
    }

    const std::vector<std::shared_ptr<NFAState>>& NFAState::getEpsilonTransitions() const {
        return epsilonTransitions;
    }

    const std::string& NFAState::getTokenName() const {
        return tokenName;
    }

    void NFAState::setTokenName(const std::string& name) {
        tokenName = name;
    }

    int NFAState::getPriority() const {
        return priority;
    }

    void NFAState::setPriority(int p) {
        priority = p;
    }

    // NFA实现
    NFA::NFA() : startState(nullptr) {}

    std::shared_ptr<NFAState> NFA::createState(bool isFinal) {
        auto state = std::make_shared<NFAState>(states.size(), isFinal);
        states.push_back(state);
        if (isFinal) {
            finalState = state;
        }
        return state;
    }

    void NFA::setStartState(std::shared_ptr<NFAState> state) {
        startState = state;
    }

    void NFA::setFinalState(std::shared_ptr<NFAState> state) {
        if (!state->isFinalState()) {
            state->setFinal(true);
            finalState = state;
        }
    }

    const std::vector<std::shared_ptr<NFAState>>& NFA::getAllStates() const {
        return states;
    }

    std::shared_ptr<NFAState> NFA::getStartState() const {
        return startState;
    }

    std::shared_ptr<NFAState> NFA::getFinalState() const {
        return finalState;
    }

    // 计算单个状态的ε闭包
    std::set<std::shared_ptr<NFAState>> NFA::epsilonClosure(std::shared_ptr<NFAState> state) const {
        std::set<std::shared_ptr<NFAState>> result;
        std::stack<std::shared_ptr<NFAState>> stack;

        // 初始状态加入结果集和栈
        result.insert(state);
        stack.push(state);

        // 使用深度优先搜索计算闭包
        while (!stack.empty()) {
            std::shared_ptr<NFAState> current = stack.top();
            stack.pop();

            // 遍历所有 ε 转移
            for (const auto& nextState : current->getEpsilonTransitions()) {
                // 如果这个状态尚未在结果集中，则添加并压入栈中
                if (result.find(nextState) == result.end()) {
                    result.insert(nextState);
                    stack.push(nextState);
                }
            }
        }

        return result;
    }

    // 辅助函数:为DFA状态设置token信息
    void NFA::setDFAStateTokenInfo(std::shared_ptr<DFAState> dfaState,
        const std::set<std::shared_ptr<NFAState>>& nfaStates) const {
        std::string tokenName;
        int highestPriority = -1;
        bool isFinal = false;

        // 调试输出
        std::cout << std::endl;
        std::cout << "    Analyzing token info for DFA state " << dfaState->getId() << ":" << std::endl;

        for (const auto &nfaState : nfaStates) {
            // 是否是全局终结状态
            if (nfaState->isFinalState()) {
                isFinal = true;
                std::cout << "      NFA state " << nfaState->getId() << " is FINAL" << std::endl;
            }

            // 检查是否有token名称
            if (!nfaState->getTokenName().empty()) {
                std::cout << "      NFA state " << nfaState->getId()
                    << " has token '" << nfaState->getTokenName()
                    << "' with priority " << nfaState->getPriority() << std::endl;

                // 选择优先级最高的token
                if (nfaState->getPriority() > highestPriority) {
                    highestPriority = nfaState->getPriority();
                    tokenName = nfaState->getTokenName();
                }
                else if (nfaState->getPriority() == highestPriority && !tokenName.empty() && tokenName != nfaState->getTokenName()) {
                    std::cout << "      Warning: Conflict detected between tokens '"
                        << tokenName << "' and '" << nfaState->getTokenName()
                        << "' with the same priority " << highestPriority << std::endl;
                }

            }
        }

        // 只有当包含tokenname的NFA状态时，才将DFA状态设为终结状态
        if (!tokenName.empty()) {
            dfaState->setFinal(true);
            dfaState->setTokenName(tokenName);
            dfaState->setPriority(highestPriority);

            std::cout << "    => DFA state " << dfaState->getId()
                << " is FINAL with token [" << tokenName << "]"
                << " (priority: " << highestPriority << ")" << std::endl;
        }
        else {
            std::cout << "    => DFA state " << dfaState->getId()
                << " is NOT final (no token found)" << std::endl;
        }
    }

    // 计算状态集合的ε闭包
    std::set<std::shared_ptr<NFAState>> NFA::epsilonClosure(const std::set<std::shared_ptr<NFAState>>& states) const {
        std::set<std::shared_ptr<NFAState>> result;
        std::stack<std::shared_ptr<NFAState>> stack;

        // 初始状态集合加入结果集和栈
        for (const auto& state : states) {
            result.insert(state);
            stack.push(state);
        }

        // 使用深度优先搜索计算闭包
        while (!stack.empty()) {
            std::shared_ptr<NFAState> current = stack.top();
            stack.pop();

            // 遍历所有 ε 转移
            for (const auto& nextState : current->getEpsilonTransitions()) {
                // 如果这个状态尚未在结果集中，则添加并压入栈中
                if (result.find(nextState) == result.end()) {
                    result.insert(nextState);
                    stack.push(nextState);
                }
            }
        }

        return result;
    }

    // 计算状态集合的转移
    std::set<std::shared_ptr<NFAState>> NFA::move(const std::set<std::shared_ptr<NFAState>>& states, char symbol) const {
        std::set<std::shared_ptr<NFAState>> result;

        // 遍历状态集合中的每个状态
        for (const auto& state : states) {
            // 获取该状态的所有转移
            const auto& transitions = state->getTransitions();

            // 查找是否有通过该符号的转移
            auto it = transitions.find(symbol);
            if (it != transitions.end()) {
                // 添加所有目标状态到结果集合
                const auto& targets = it->second;
                result.insert(targets.begin(), targets.end());
            }
        }

        return result;
    }

    // 使用子集构造法将NFA转换为DFA
    std::shared_ptr<DFA> NFA::toDFA() const {
        std::shared_ptr<DFA> dfa = std::make_shared<DFA>();

        if (!startState) {
            return dfa; // 空NFA，返回空DFA
        }

        // 收集所有输入符号(不含ε)
        std::set<char> inputSymbols;
        for (const auto& state : states) {
            for (const auto& [symbol, targets] : state->getTransitions()) {
                inputSymbols.insert(symbol);
            }
        }

        // std::cout << "Input symbols: ";
        // for (char c : inputSymbols) {
        //     std::cout << c << " ";
        // }
        // std::cout << std::endl;

        // 映射NFA状态集到DFA状态
        std::map<std::set<std::shared_ptr<NFAState>>, std::shared_ptr<DFAState>> stateMap;

        // 待处理的NFA状态集队列
        std::queue<std::set<std::shared_ptr<NFAState>>> unmarkedStates;

        // 计算初始状态的ε闭包
        std::set<std::shared_ptr<NFAState>> startClosure = epsilonClosure(startState);
        auto dfaStartState = dfa->createState(startClosure);
        dfa->setStartState(dfaStartState);

        stateMap[startClosure] = dfaStartState;
        unmarkedStates.push(startClosure);

        // 设置DFA状态的token信息
        setDFAStateTokenInfo(dfaStartState, startClosure);

        std::cout << "Starting subset construction..." << std::endl;
        std::cout << "Initial DFA state " << dfaStartState->getId()
            << " contains " << startClosure.size() << " NFA states" << std::endl;

        // 子集构造法主循环
        int stateCount = 0;
        while (!unmarkedStates.empty()) {
            std::set<std::shared_ptr<NFAState>> currentNFAStates = unmarkedStates.front();
            unmarkedStates.pop(); // 标记为已处理

            std::shared_ptr<DFAState> currentDFAState = stateMap[currentNFAStates];
            stateCount++;

            // 调试输出
            std::cout << "\nProcessing DFA state " << currentDFAState->getId();
            //     << " (NFA states: ";
            // for (const auto& s : currentNFAStates) {
            //     std::cout << s->getId() << " ";
            // }
            // std::cout << ")" << std::endl;

            // 对于每个输入符号
            for (char symbol : inputSymbols) {
                // 计算move
                std::set<std::shared_ptr<NFAState>> moveResult = move(currentNFAStates, symbol);
                if (moveResult.empty()) {
                    continue; // 没有转移，跳过
                }
                // 计算ε闭包
                std::set<std::shared_ptr<NFAState>> closureResult = epsilonClosure(moveResult);

                // 调试输出
                // std::cout << "  On symbol '" << symbol << "' -> DFA state with NFA states: ";
                // for (const auto& s : closureResult) {
                //     std::cout << s->getId() << " ";
                // }

                // 检查该状态集是否已存在
                std::shared_ptr<DFAState> newDFAState;
                if (stateMap.find(closureResult) == stateMap.end()) {
                    // 新状态，创建DFA状态
                    newDFAState = dfa->createState(closureResult);
                    stateMap[closureResult] = newDFAState;
                    unmarkedStates.push(closureResult);

                    // 设置DFA状态的token信息
                    setDFAStateTokenInfo(newDFAState, closureResult);

                    // 调试输出
                    std::cout << " => Created new DFA state " << newDFAState->getId() << std::endl;
                }
                else {
                    newDFAState = stateMap[closureResult];
                    // std::cout << " (existing state " << newDFAState->getId() << ")" << std::endl;
                }
                // std::cout << std::endl;

                // 添加DFA转移
                currentDFAState->addTransition(symbol, newDFAState);
            }
        }

        // 收集所有终结状态
        for (const auto& [nfaSet, dfaState] : stateMap) {
            if (dfaState->isFinalState()) {
                dfa->addFinalState(dfaState);
            }
        }

        // 调试输出
        std::cout << std::endl;
        std::cout << "\nSubset construction completed!" << std::endl;
        std::cout << "Total DFA states: " << stateMap.size() << std::endl;
        std::cout << "Total final states: " << dfa->getFinalStates().size() << std::endl;
        std::cout << std::endl;

        return dfa;
    }

} // namespace Compiler
