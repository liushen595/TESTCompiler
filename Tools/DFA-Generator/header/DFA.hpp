#ifndef DFA_HPP
#define DFA_HPP

#include <vector>
#include <set>
#include <map>
#include <string>
#include <memory>
#include <unordered_map>
#include <unordered_set>

namespace Compiler {

    // 前向声明
    class NFA;
    class NFAState;

    // DFA的状态类
    class DFAState {
    public:
        DFAState(int id, const std::set<NFAState*>& nfaStates);
        ~DFAState() = default;

        int getId() const;
        bool isFinalState() const;
        void setFinal(bool final);

        // 添加一个转移
        void addTransition(char symbol, DFAState* target);

        // 获取所有可能的转移
        const std::map<char, DFAState*>& getTransitions() const;

        // 获取对应的NFA状态集合
        const std::set<NFAState*>& getNFAStates() const;

        // 获取该状态接受的词法单元名称
        const std::string& getTokenName() const;

        // 设置该状态接受的词法单元名称
        void setTokenName(const std::string& name);

        // 获取该状态的优先级
        int getPriority() const;

        // 设置该状态的优先级
        void setPriority(int priority);

    private:
        int id;                              // 状态ID
        bool finalState;                     // 是否为终结状态
        std::map<char, DFAState*> transitions;  // 转移函数
        std::set<NFAState*> nfaStates;       // 对应的NFA状态集合
        std::string tokenName;               // 接受的词法单元名称
        int priority;                        // 状态优先级
    };

    // DFA类
    class DFA {
    public:
        DFA();
        ~DFA();

        // 创建一个新状态
        DFAState* createState(const std::set<NFAState*>& nfaStates);

        // 设置初始状态
        void setStartState(DFAState* state);

        // 添加一个终结状态
        void addFinalState(DFAState* state);

        // 获取所有状态
        const std::vector<DFAState*>& getAllStates() const;

        // 获取初始状态
        DFAState* getStartState() const;

        // 获取所有终结状态
        const std::vector<DFAState*>& getFinalStates() const;

        // 最小化DFA
        void minimize();

        // 生成DFA表
        void generateTable(std::map<int, std::map<char, int>>& transitionTable,
            std::map<int, std::string>& acceptStates) const;

        // 导出DFA表到头文件
        bool exportToHeaderFile(const std::string& filePath) const;

    private:
        std::vector<DFAState*> states;         // 所有状态
        DFAState* startState;                  // 初始状态
        std::vector<DFAState*> finalStates;    // 终结状态集合

        // 辅助函数：查找或创建等价状态
        DFAState* findOrCreateState(const std::set<NFAState*>& nfaStates);

        // 辅助函数：划分等价类
        std::vector<std::set<DFAState*>> partitionStates() const;

        // 辅助函数：检查两个状态是否等价
        bool areEquivalent(DFAState* state1, DFAState* state2,
            const std::vector<std::set<DFAState*>>& partition) const;
    };

} // namespace Compiler

#endif // DFA_HPP
