#include "NFA.hpp"
#include "DFA.hpp"
#include <queue>
#include <algorithm>
#include <iostream>

namespace Compiler {

    // NFAState实现
    NFAState::NFAState(int id, bool isFinal) : id(id), finalState(isFinal), priority(0) {}

    int NFAState::getId() const {
        return id;
    }

    bool NFAState::isFinalState() const {
        return finalState;
    }

    void NFAState::setFinal(bool final) {
        finalState = final;
    }

    void NFAState::addTransition(char symbol, NFAState* target) {
        transitions[symbol].push_back(target);
    }

    void NFAState::addEpsilonTransition(NFAState* target) {
        epsilonTransitions.push_back(target);
    }

    const std::map<char, std::vector<NFAState*>>& NFAState::getTransitions() const {
        return transitions;
    }

    const std::vector<NFAState*>& NFAState::getEpsilonTransitions() const {
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

    NFA::~NFA() {
        // 释放所有状态资源
        for (auto state : states) {
            delete state;
        }
    }

    NFAState* NFA::createState(bool isFinal) {
        NFAState* state = new NFAState(states.size(), isFinal);
        states.push_back(state);
        if (isFinal) {
            finalStates.push_back(state);
        }
        return state;
    }

    void NFA::setStartState(NFAState* state) {
        startState = state;
    }

    void NFA::addFinalState(NFAState* state) {
        if (!state->isFinalState()) {
            state->setFinal(true);
            finalStates.push_back(state);
        }
    }

    const std::vector<NFAState*>& NFA::getAllStates() const {
        return states;
    }

    NFAState* NFA::getStartState() const {
        return startState;
    }

    const std::vector<NFAState*>& NFA::getFinalStates() const {
        return finalStates;
    }

    std::set<NFAState*> NFA::epsilonClosure(NFAState* state) const {
        // 计算单个状态的ε闭包

        // TODO: 实现计算ε闭包的逻辑

        std::set<NFAState*> result;
        // 暂时只添加当前状态，后续需要实现
        result.insert(state);
        return result;
    }

    std::set<NFAState*> NFA::epsilonClosure(const std::set<NFAState*>& states) const {
        // 计算状态集合的ε闭包

        // TODO: 实现计算状态集合ε闭包的逻辑

        std::set<NFAState*> result;
        // 暂时只添加当前状态集合，后续需要实现
        result.insert(states.begin(), states.end());
        return result;
    }

    std::set<NFAState*> NFA::move(const std::set<NFAState*>& states, char symbol) const {
        // 计算状态集合的转移

        // TODO: 实现计算状态集合转移的逻辑

        std::set<NFAState*> result;
        return result;
    }

    std::shared_ptr<DFA> NFA::toDFA() const {
        // 使用子集构造法将NFA转换为DFA

        // TODO: 实现子集构造算法

        return std::make_shared<DFA>();
    }

} // namespace Compiler
