#ifndef NFA_HPP
#define NFA_HPP

#include <vector>
#include <set>
#include <map>
#include <string>
#include <memory>
#include <unordered_map>

namespace Compiler {

    // 前向声明
    class DFA;

    // NFA的状态类
    class NFAState {
    public:
        NFAState(int id, bool isFinal = false);
        ~NFAState() = default;

        int getId() const;
        bool isFinalState() const;
        void setFinal(bool final);

        // 添加一个转移
        void addTransition(char symbol, NFAState* target);

        // 添加一个ε转移
        void addEpsilonTransition(NFAState* target);

        // 获取所有可能的转移
        const std::map<char, std::vector<NFAState*>>& getTransitions() const;

        // 获取所有ε转移
        const std::vector<NFAState*>& getEpsilonTransitions() const;

        // 获取该状态接受的词法单元名称
        const std::string& getTokenName() const;

        // 设置该状态接受的词法单元名称
        void setTokenName(const std::string& name);

        // 获取该状态的优先级
        int getPriority() const;

        // 设置该状态的优先级
        void setPriority(int priority);

    private:
        int id;                                     // 状态ID
        bool finalState;                            // 是否为终结状态
        std::map<char, std::vector<NFAState*>> transitions;  // 非ε转移
        std::vector<NFAState*> epsilonTransitions;  // ε转移
        std::string tokenName;                      // 接受的词法单元名称
        int priority;                              // 状态优先级
    };

    // NFA类
    class NFA {
    private:
        std::vector<NFAState*> states;         // 所有状态
        NFAState* startState;                  // 初始状态
        std::vector<NFAState*> finalStates;    // 终结状态集合
    public:
        NFA();
        ~NFA();

        // 创建一个新状态
        NFAState* createState(bool isFinal = false);

        // 设置初始状态
        void setStartState(NFAState* state);

        // 添加一个终结状态
        void addFinalState(NFAState* state);

        // 获取所有状态
        const std::vector<NFAState*>& getAllStates() const;

        // 获取初始状态
        NFAState* getStartState() const;

        // 获取所有终结状态
        const std::vector<NFAState*>& getFinalStates() const;

        // 计算状态的ε闭包
        std::set<NFAState*> epsilonClosure(NFAState* state) const;
        std::set<NFAState*> epsilonClosure(const std::set<NFAState*>& states) const;

        // 计算状态集合的转移
        std::set<NFAState*> move(const std::set<NFAState*>& states, char symbol) const;

        // 使用子集构造法将NFA转换为DFA
        std::shared_ptr<DFA> toDFA() const;
    };

} // namespace Compiler

#endif // NFA_HPP
