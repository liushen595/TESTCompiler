#ifndef REGEX_ENGINE_HPP
#define REGEX_ENGINE_HPP

#include <string>
#include <vector>
#include <map>
#include <set>
#include <memory>
#include "NFA.hpp"

namespace Compiler {

    // 正则表达式规则
    // struct RegexRule {
    //     std::string pattern;       // 正则表达式模式
    //     std::string tokenName;     // 对应的词法单元名称
    //     // int priority;              // 规则优先级
    //     RegexRule(const std::string& p, const std::string& t) : pattern(p), tokenName(t) {}
    // };

    // 正则表达式引擎类，负责解析正则表达式并生成NFA
    class RegexEngine {
    private:
        std::unordered_map<std::string, std::string> regexrules;
        // std::unordered_map<std::string, std::string> macros;

        // 辅助函数：解析正则表达式，将其转换为内部表示
        void preprocessRegex(std::unordered_map<std::string, std::string> &regexrules);

        // 辅助函数：将中缀表达式转换为后缀表达式
        std::string infixToPostfix(const std::string& regex);

        // 使用MYT算法将正则表达式转换为NFA
        std::shared_ptr<NFA> regexToNFA(std::unordered_map<std::string, std::string> &regexrules);

        // 实现MYT算法的各个函数
        std::shared_ptr<NFA> createBasicNFA(char c);
        std::shared_ptr<NFA> createConcatenation(std::shared_ptr<NFA> first, std::shared_ptr<NFA> second);
        std::shared_ptr<NFA> createUnion(std::shared_ptr<NFA> first, std::shared_ptr<NFA> second);
        std::shared_ptr<NFA> createKleeneClosure(std::shared_ptr<NFA> nfa);
        std::shared_ptr<NFA> createPositiveClosure(std::shared_ptr<NFA> nfa);
        // std::shared_ptr<NFA> createOptional(std::shared_ptr<NFA> nfa);
    public:
        RegexEngine() = default;
        ~RegexEngine() = default;

        // 从文件加载正则表达式规则
        bool loadRulesFromFile(const std::string& filePath);

        // 获取已加载的规则列表
        const std::unordered_map<std::string, std::string>& getRules() const;

        // 将所有规则合并成一个大的NFA
        std::shared_ptr<NFA> buildCombinedNFA();
    };

} // namespace Compiler

#endif // REGEX_ENGINE_HPP
