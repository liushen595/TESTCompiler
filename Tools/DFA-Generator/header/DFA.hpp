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
    class DFAState;

    // DFA的状态类
    class DFAState : public std::enable_shared_from_this<DFAState> {
    public:
        DFAState(int id, const std::set<std::shared_ptr<NFAState>>& nfaStates);
        ~DFAState() = default;

        int getId() const;
        bool isFinalState() const;
        void setFinal(bool final);

        // 添加一个转移
        void addTransition(char symbol, std::shared_ptr<DFAState> target);

        // 获取所有可能的转移
        const std::map<char, std::shared_ptr<DFAState>>& getTransitions() const;

        // 获取对应的NFA状态集合
        const std::set<std::shared_ptr<NFAState>>& getNFAStates() const;

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
        std::map<char, std::shared_ptr<DFAState>> transitions;  // 转移函数
        std::set<std::shared_ptr<NFAState>> nfaStates;       // 对应的NFA状态集合
        std::string tokenName;               // 接受的词法单元名称
        int priority;                        // 状态优先级
    };

    // DFA类
    class DFA {
    public:
        DFA();
        ~DFA() = default; // 使用智能指针，无需手动释放内存

        // 创建一个新状态
        std::shared_ptr<DFAState> createState(const std::set<std::shared_ptr<NFAState>>& nfaStates);

        // 设置初始状态
        void setStartState(std::shared_ptr<DFAState> state);

        // 添加一个终结状态
        void addFinalState(std::shared_ptr<DFAState> state);

        // 获取所有状态
        const std::vector<std::shared_ptr<DFAState>>& getAllStates() const;

        // 获取初始状态
        std::shared_ptr<DFAState> getStartState() const;

        // 获取所有终结状态
        const std::vector<std::shared_ptr<DFAState>>& getFinalStates() const;

        // 最小化DFA
        void minimize();

        // 生成DFA表
        void generateTable(std::map<int, std::map<char, int>>& transitionTable,
            std::map<int, std::string>& acceptStates) const;

        // 导出DFA表到头文件
        bool exportToHeaderFile(const std::string& filePath) const;

    private:
        std::vector<std::shared_ptr<DFAState>> states;         // 所有状态
        std::shared_ptr<DFAState> startState;                  // 初始状态
        std::vector<std::shared_ptr<DFAState>> finalStates;    // 终结状态集合

        // 辅助函数：查找或创建等价状态
        std::shared_ptr<DFAState> findOrCreateState(const std::set<std::shared_ptr<NFAState>>& nfaStates);

        // 辅助函数：划分等价类
        std::vector<std::set<std::shared_ptr<DFAState>>> partitionStates() const;

        // 辅助函数：检查两个状态是否等价
        bool areEquivalent(std::shared_ptr<DFAState> state1, std::shared_ptr<DFAState> state2,
            const std::vector<std::set<std::shared_ptr<DFAState>>>& partition) const;
    };

} // namespace Compiler

#endif // DFA_HPP
