#include "DFA.hpp"
#include "NFA.hpp"
#include <queue>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>

namespace Compiler {

    // DFAState实现
    DFAState::DFAState(size_t id, const std::set<std::shared_ptr<NFAState>>& nfaStates)
        : id(id), finalState(false), nfaStates(nfaStates), priority(0) {
        // 检查是否包含NFA终结状态，如果包含则将该DFA状态设为终结状态
        // for (auto nfaState : nfaStates) {
        //     if (nfaState->isFinalState()) {
        //         finalState = true;
        //         // 设置词法单元名称和优先级
        //         if (nfaState->getPriority() > priority) {
        //             priority = nfaState->getPriority();
        //             tokenName = nfaState->getTokenName();
        //         }
        //     }
        // }
    }

    size_t DFAState::getId() const {
        return id;
    }

    bool DFAState::isFinalState() const {
        return finalState;
    }

    void DFAState::setFinal(bool final) {
        finalState = final;
    }

    void DFAState::addTransition(char symbol, std::shared_ptr<DFAState> target) {
        transitions[symbol] = target;
    }

    const std::map<char, std::shared_ptr<DFAState>>& DFAState::getTransitions() const {
        return transitions;
    }

    const std::set<std::shared_ptr<NFAState>>& DFAState::getNFAStates() const {
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

    std::shared_ptr<DFAState> DFA::createState(const std::set<std::shared_ptr<NFAState>>& nfaStates) {
        auto state = std::make_shared<DFAState>(states.size(), nfaStates);
        states.push_back(state);
        // 这里不自动添加到终结状态列表
        // if (state->isFinalState()) {
        //     finalStates.push_back(state);
        // }
        return state;
    }

    void DFA::setStartState(std::shared_ptr<DFAState> state) {
        startState = state;
    }

    void DFA::addFinalState(std::shared_ptr<DFAState> state) {
        if (state->isFinalState()) {
            state->setFinal(true);
            finalStates.push_back(state);
        }
    }

    const std::vector<std::shared_ptr<DFAState>>& DFA::getAllStates() const {
        return states;
    }

    std::shared_ptr<DFAState> DFA::getStartState() const {
        return startState;
    }

    const std::vector<std::shared_ptr<DFAState>>& DFA::getFinalStates() const {
        return finalStates;
    }

    // 根据划分找到状态所在的子集编号
    int DFA::findPartitionIndex(std::shared_ptr<DFAState> state,
        const std::vector<std::set<std::shared_ptr<DFAState>>>& partition) const {
        for (size_t i = 0; i < partition.size(); ++i) {
            if (partition[i].find(state) != partition[i].end()) {
                return static_cast<int>(i);
            }
        }
        return -1;  // 未找到
    }

    // 划分等价类
    std::vector<std::set<std::shared_ptr<DFAState>>> DFA::partitionStates() const {
        // 初始划分：非终结状态一组，终结状态按tokenName分组
        std::set<std::shared_ptr<DFAState>> nonFinalSet;
        std::map<std::string, std::set<std::shared_ptr<DFAState>>> finalSetsByToken;

        for (const std::shared_ptr<DFAState> &state : states) {
            if (state->isFinalState()) {
                // 终结状态按tokenName分组
                std::string token = state->getTokenName();
                finalSetsByToken[token].insert(state);
            }
            else {
                nonFinalSet.insert(state);
            }
        }

        std::vector<std::set<std::shared_ptr<DFAState>>> partition;
        // 先加入非终结状态组
        if (!nonFinalSet.empty()) {
            partition.push_back(nonFinalSet);
        }
        // 再加入各个终结状态组（按tokenName分开）
        for (const std::pair<const std::string, std::set<std::shared_ptr<DFAState>>> &entry : finalSetsByToken) {
            if (!entry.second.empty()) {
                partition.push_back(entry.second);
            }
        }

        // 所有输入符号
        std::set<char> inputSymbols;
        for (const auto &state : states) {
            for (const auto &trans : state->getTransitions()) {
                inputSymbols.insert(trans.first);
            }
        }

        // 迭代划分
        bool changed = true;
        while (changed) {
            changed = false;
            std::vector<std::set<std::shared_ptr<DFAState>>> newPartition;

            // 对每个划分进行细化
            for (const auto &part : partition) {
                std::map<std::vector<int>, std::set<std::shared_ptr<DFAState>>> subsets;

                // 对划分中的每个状态计算签名
                for (const std::shared_ptr<DFAState> &state : part) {
                    std::vector<int> signature;

                    // 对每个输入符号，记录转移到哪个子集
                    for (char symbol : inputSymbols) {
                        auto it = state->getTransitions().find(symbol);
                        if (it != state->getTransitions().end()) {
                            // 找到转移，记录目标状态所在的划分编号
                            std::shared_ptr<DFAState> targetState = it->second;
                            int targetIndex = findPartitionIndex(targetState, partition);
                            signature.push_back(targetIndex);
                        }
                        else {
                            // 没有转移，使用-1表示
                            signature.push_back(-1);
                        }
                    }
                    // 根据签名将状态分组
                    subsets[signature].insert(state);
                }
                if (subsets.size() > 1) {
                    changed = true; // 划分被细化
                }

                // 所有子集加入新的划分
                for (const auto &entry : subsets) {
                    newPartition.push_back(entry.second);
                }
            }
            partition = newPartition; // 更新划分
        }

        return partition;
    }

    // std::shared_ptr<DFAState> DFA::findOrCreateState(const std::set<std::shared_ptr<NFAState>>& nfaStates) {
    //     // 查找或创建等价状态

    //     // TODO: 实现查找或创建等价状态的逻辑

    //     return nullptr; // 暂时返回空指针，后续需要实现
    // }

    // 检查两个状态是否等价
    bool DFA::areEquivalent(std::shared_ptr<DFAState> state1,
        std::shared_ptr<DFAState> state2,
        const std::vector<std::set<std::shared_ptr<DFAState>>>&partition) const {

        // 首先检查是否在同一个划分中
        int index1 = findPartitionIndex(state1, partition);
        int index2 = findPartitionIndex(state2, partition);

        return index1 != -1 && index1 == index2;
    }

    // 最小化DFA
    void DFA::minimize() {
        if (states.empty()) {
            return; // 空DFA，无需最小化
        }

        std::cout << "Starting DFA minimization..." << std::endl;
        std::cout << "Original states: " << states.size() << std::endl;

        // 1. 划分等价类
        std::vector<std::set<std::shared_ptr<DFAState>>> finalPartition = partitionStates();

        std::cout << "Final partition has " << finalPartition.size() << " groups" << std::endl;

        // 2. 为每个划分组选择一个代表状态
        std::map<std::shared_ptr<DFAState>, std::shared_ptr<DFAState>> oldToNewRepresentative;
        std::vector<std::shared_ptr<DFAState>> representatives;

        for (const std::set<std::shared_ptr<DFAState>> &group : finalPartition) {
            std::shared_ptr<DFAState> representative = *group.begin();

            // 如果组中包含起始状态，优先选择起始状态作为代表
            for (const std::shared_ptr<DFAState> &state : group) {
                if (state == startState) {
                    representative = state;
                    break;
                }
            }

            representatives.push_back(representative);

            // 建立旧状态到代表状态的映射
            for (const std::shared_ptr<DFAState> &state : group) {
                oldToNewRepresentative[state] = representative;
            }
        }

        // 3. 创建新的DFA状态（克隆代表状态）
        std::map<std::shared_ptr<DFAState>, std::shared_ptr<DFAState>> RepToNewState;
        std::vector<std::shared_ptr<DFAState>> newStates;

        for (size_t i = 0; i < representatives.size(); ++i) {
            std::shared_ptr<DFAState> oldRep = representatives[i];
            // 创建新状态，使用空的NFA状态集（因为最小化后不再需要）
            std::set<std::shared_ptr<NFAState>> emptyNFAStates;
            std::shared_ptr<DFAState> newState = std::make_shared<DFAState>(i, emptyNFAStates);

            // 复制代表状态的属性
            newState->setFinal(oldRep->isFinalState());
            newState->setTokenName(oldRep->getTokenName());
            newState->setPriority(oldRep->getPriority());

            RepToNewState[oldRep] = newState;
            newStates.push_back(newState);
        }

        // 4. 重建转移函数
        for (size_t i = 0; i < representatives.size(); ++i) {
            std::shared_ptr<DFAState> oldRep = representatives[i];
            std::shared_ptr<DFAState> newState = newStates[i];

            // 遍历旧代表状态的所有转移
            const std::map<char, std::shared_ptr<DFAState>> &transitions = oldRep->getTransitions();
            for (const std::pair<const char, std::shared_ptr<DFAState>> &transition : transitions) {
                char symbol = transition.first;
                std::shared_ptr<DFAState> oldTarget = transition.second;

                // 找到旧目标状态对应的代表状态
                std::shared_ptr<DFAState> targetRep = oldToNewRepresentative[oldTarget];
                // 找到代表状态对应的新状态
                std::shared_ptr<DFAState> newTarget = RepToNewState[targetRep];
                // 添加转移到新状态
                newState->addTransition(symbol, newTarget);
            }
        }

        // 5. 确定新的起始状态
        std::shared_ptr<DFAState> oldStartRep = oldToNewRepresentative[startState];
        std::shared_ptr<DFAState> newStartState = RepToNewState[oldStartRep];

        // 6. 收集新的终结状态
        std::vector<std::shared_ptr<DFAState>> newFinalStates;
        for (const std::shared_ptr<DFAState> &state : newStates) {
            if (state->isFinalState()) {
                newFinalStates.push_back(state);
            }
        }

        // 7. 替换当前DFA的所有状态
        states = newStates;
        startState = newStartState;
        finalStates = newFinalStates;

        std::cout << "Minimized states: " << states.size() << std::endl;
        std::cout << "DFA minimization completed!" << std::endl;
    }

    void DFA::generateTable(std::map<int, std::map<char, int>>&transitionTable,
        std::map<int, std::string>&acceptStates) const {
        // 生成DFA表
        transitionTable.clear();
        acceptStates.clear();

        // 遍历所有状态
        for (const std::shared_ptr<DFAState> &state : states) {
            int stateId = static_cast<int>(state->getId());

            // 添加转移
            std::map<char, int> stateTransitions;
            const std::map<char, std::shared_ptr<DFAState>> &transitions = state->getTransitions();
            for (const std::pair<const char, std::shared_ptr<DFAState>> &transition : transitions) {
                char symbol = transition.first;
                std::shared_ptr<DFAState> target = transition.second;
                int targetId = static_cast<int>(target->getId());
                stateTransitions[symbol] = targetId;
            }
            transitionTable[stateId] = stateTransitions;

            // 如果是终结状态，记录其接受的tokenName
            if (state->isFinalState()) {
                acceptStates[stateId] = state->getTokenName();
            }
        }
    }

    bool DFA::exportToHeaderFile(const std::string & filePath) const {
        // 导出DFA表到头文件
        std::ofstream outFile(filePath);
        if (!outFile.is_open()) {
            std::cerr << "Error: Cannot open file " << filePath << " for writing" << std::endl;
            return false;
        }

        // 生成DFA表
        std::map<int, std::map<char, int>> transitionTable;
        std::map<int, std::string> acceptStates;
        generateTable(transitionTable, acceptStates);

        // 写入头文件保护宏
        outFile << "#ifndef DFA_TABLES_HPP\n";
        outFile << "#define DFA_TABLES_HPP\n\n";
        outFile << "#include <map>\n";
        outFile << "#include <string>\n\n";
        outFile << "namespace Compiler {\n\n";

        // 写入起始状态
        outFile << "// DFA start state ID\n";
        outFile << "constexpr int DFA_START_STATE = " << (startState ? static_cast<int>(startState->getId()) : 0) << ";\n\n";

        // 写入状态总数
        outFile << "// DFA states count\n";
        outFile << "constexpr int DFA_STATE_COUNT = " << states.size() << ";\n\n";

        // 写入转移表
        outFile << "// DFA transition table: [current state ID][input symbol] -> target state ID\n";
        outFile << "const std::map<int, std::map<char, int>> DFA_TRANSITION_TABLE = {\n";

        bool firstState = true;
        for (const std::pair<const int, std::map<char, int>> &stateEntry : transitionTable) {
            if (!firstState) {
                outFile << ",\n";
            }
            firstState = false;

            int stateId = stateEntry.first;
            const std::map<char, int> &transitions = stateEntry.second;

            outFile << "    {" << stateId << ", {\n";

            bool firstTrans = true;
            for (const std::pair<const char, int> &trans : transitions) {
                if (!firstTrans) {
                    outFile << ",\n";
                }
                firstTrans = false;

                char symbol = trans.first;
                int targetId = trans.second;

                // 处理特殊字符
                if (symbol == '\n') {
                    outFile << "        {'\\n', " << targetId << "}";
                }
                else if (symbol == '\t') {
                    outFile << "        {'\\t', " << targetId << "}";
                }
                else if (symbol == '\r') {
                    outFile << "        {'\\r', " << targetId << "}";
                }
                else if (symbol == '\\') {
                    outFile << "        {'\\\\', " << targetId << "}";
                }
                else if (symbol == '\'') {
                    outFile << "        {'\\'', " << targetId << "}";
                }
                else if (symbol == '\"') {
                    outFile << "        {'\\\"', " << targetId << "}";
                }
                else if (isprint(symbol)) {
                    outFile << "        {'" << symbol << "', " << targetId << "}";
                }
                else {
                    // 不可打印字符用十六进制表示
                    outFile << "        {'\\x" << std::hex << static_cast<int>(static_cast<unsigned char>(symbol)) << "', " << std::dec << targetId << "}";
                }
            }
            outFile << "\n    }}";
        }
        outFile << "\n};\n\n";

        // 写入接受状态表
        outFile << "// DFA accept states table: [state ID] -> Token name\n";
        outFile << "const std::map<int, std::string> DFA_ACCEPT_STATES = {\n";

        bool firstAccept = true;
        for (const std::pair<const int, std::string> &entry : acceptStates) {
            if (!firstAccept) {
                outFile << ",\n";
            }
            firstAccept = false;

            int stateId = entry.first;
            const std::string &tokenName = entry.second;
            outFile << "    {" << stateId << ", \"" << tokenName << "\"}";
        }
        outFile << "\n};\n\n";

        // 结束命名空间和头文件保护
        outFile << "} // namespace Compiler\n\n";
        outFile << "#endif // DFA_TABLES_HPP\n";

        outFile.close();

        std::cout << "DFA tables exported to " << filePath << std::endl;
        std::cout << "Total states: " << states.size() << std::endl;
        std::cout << "Accept states: " << acceptStates.size() << std::endl;

        return true;
    }

} // namespace Compiler
