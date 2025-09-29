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
    class NFAState;

    // NFA的状态类
    class NFAState : public std::enable_shared_from_this<NFAState> {
    private:
        int id;                                     // 状态ID
        bool finalState;                            // 是否为终结状态
        std::map<char, std::vector<std::shared_ptr<NFAState>>> transitions;  // 非ε转移
        std::vector<std::shared_ptr<NFAState>> epsilonTransitions;  // ε转移
        std::string tokenName;                      // 接受的词法单元名称
        int priority;                              // 状态优先级
    public:
        NFAState(int id, bool isFinal = false);
        ~NFAState() = default;

        int getId() const;
        bool isFinalState() const;
        void setFinal(bool final);

        // 添加一个转移
        void addTransition(char symbol, std::shared_ptr<NFAState> target);

        // 添加一个ε转移
        void addEpsilonTransition(std::shared_ptr<NFAState> target);

        // 获取所有可能的转移
        const std::map<char, std::vector<std::shared_ptr<NFAState>>>& getTransitions() const;

        // 获取所有ε转移
        const std::vector<std::shared_ptr<NFAState>>& getEpsilonTransitions() const;

        // 获取该状态接受的词法单元名称
        const std::string& getTokenName() const;

        // 设置该状态接受的词法单元名称
        void setTokenName(const std::string& name);

        // 获取该状态的优先级
        int getPriority() const;

        // 设置该状态的优先级
        void setPriority(int priority);
    };

    // NFA类
    class NFA {
    private:
        std::vector<std::shared_ptr<NFAState>> states;         // 所有状态
        std::shared_ptr<NFAState> startState;                  // 初始状态
        std::shared_ptr<NFAState> finalState;                 // 终结状态
    public:
        NFA();
        ~NFA() = default; // 使用智能指针，无需手动释放内存

        // 创建一个新状态
        std::shared_ptr<NFAState> createState(bool isFinal = false);

        // 设置初始状态
        void setStartState(std::shared_ptr<NFAState> state);

        // 设置终结状态
        void setFinalState(std::shared_ptr<NFAState> state);

        // 获取所有状态
        const std::vector<std::shared_ptr<NFAState>>& getAllStates() const;

        // 获取初始状态
        std::shared_ptr<NFAState> getStartState() const;

        // 获取终结状态
        std::shared_ptr<NFAState> getFinalState() const;

        // 计算状态的ε闭包
        std::set<std::shared_ptr<NFAState>> epsilonClosure(std::shared_ptr<NFAState> state) const;
        std::set<std::shared_ptr<NFAState>> epsilonClosure(const std::set<std::shared_ptr<NFAState>>& states) const;

        // 计算状态集合的转移
        std::set<std::shared_ptr<NFAState>> move(const std::set<std::shared_ptr<NFAState>>& states, char symbol) const;

        // 使用子集构造法将NFA转换为DFA
        std::shared_ptr<DFA> toDFA() const;
    };

} // namespace Compiler

#endif // NFA_HPP
