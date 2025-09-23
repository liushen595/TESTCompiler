#include "DFA.hpp"
#include "NFA.hpp"
#include <queue>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>

namespace Compiler {

    // DFAState实现
    DFAState::DFAState(int id, const std::set<NFAState*>& nfaStates)
        : id(id), finalState(false), nfaStates(nfaStates), priority(0) {
        // 检查是否包含NFA终结状态，如果包含则将该DFA状态设为终结状态
        for (auto nfaState : nfaStates) {
            if (nfaState->isFinalState()) {
                finalState = true;
                // 设置词法单元名称和优先级
                if (nfaState->getPriority() > priority) {
                    priority = nfaState->getPriority();
                    tokenName = nfaState->getTokenName();
                }
            }
        }
    }

    int DFAState::getId() const {
        return id;
    }

    bool DFAState::isFinalState() const {
        return finalState;
    }

    void DFAState::setFinal(bool final) {
        finalState = final;
    }

    void DFAState::addTransition(char symbol, DFAState* target) {
        transitions[symbol] = target;
    }

    const std::map<char, DFAState*>& DFAState::getTransitions() const {
        return transitions;
    }

    const std::set<NFAState*>& DFAState::getNFAStates() const {
        return nfaStates;
    }

    const std::string& DFAState::getTokenName() const {
        return tokenName;
    }

    void DFAState::setTokenName(const std::string& name) {
        tokenName = name;
    }

    int DFAState::getPriority() const {
        return priority;
    }

    void DFAState::setPriority(int p) {
        priority = p;
    }

    // DFA实现
    DFA::DFA() : startState(nullptr) {}

    DFA::~DFA() {
        // 释放所有状态资源
        for (auto state : states) {
            delete state;
        }
    }

    DFAState* DFA::createState(const std::set<NFAState*>& nfaStates) {
        DFAState* state = new DFAState(states.size(), nfaStates);
        states.push_back(state);
        if (state->isFinalState()) {
            finalStates.push_back(state);
        }
        return state;
    }

    void DFA::setStartState(DFAState* state) {
        startState = state;
    }

    void DFA::addFinalState(DFAState* state) {
        if (!state->isFinalState()) {
            state->setFinal(true);
            finalStates.push_back(state);
        }
    }

    const std::vector<DFAState*>& DFA::getAllStates() const {
        return states;
    }

    DFAState* DFA::getStartState() const {
        return startState;
    }

    const std::vector<DFAState*>& DFA::getFinalStates() const {
        return finalStates;
    }

    DFAState* DFA::findOrCreateState(const std::set<NFAState*>& nfaStates) {
        // 查找或创建等价状态

        // TODO: 实现查找或创建等价状态的逻辑

        return nullptr; // 暂时返回空指针，后续需要实现
    }

    void DFA::minimize() {
        // 最小化DFA

        // TODO: 实现最小化DFA的逻辑（Hopcroft算法或其他方法）
    }

    std::vector<std::set<DFAState*>> DFA::partitionStates() const {
        // 划分等价类

        // TODO: 实现划分等价类的逻辑

        std::vector<std::set<DFAState*>> result;
        return result;
    }

    bool DFA::areEquivalent(DFAState* state1, DFAState* state2,
        const std::vector<std::set<DFAState*>>& partition) const {
        // 检查两个状态是否等价

        // TODO: 实现检查状态等价的逻辑

        return false;
    }

    void DFA::generateTable(std::map<int, std::map<char, int>>& transitionTable,
        std::map<int, std::string>& acceptStates) const {
        // 生成DFA表

        // TODO: 实现生成DFA表的逻辑
    }

    bool DFA::exportToHeaderFile(const std::string& filePath) const {
        // 导出DFA表到头文件

        // TODO: 实现导出DFA表到头文件的逻辑

        return true;
    }

} // namespace Compiler
