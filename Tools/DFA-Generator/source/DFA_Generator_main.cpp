#include "RegexEngine.hpp"
#include "NFA.hpp"
#include "DFA.hpp"
#include <iostream>
#include <string>
#include <memory>

int main(/*int argc, char* argv[]*/) {
    // if (argc < 3) {
    //     std::cout << "Usage: " << argv[0] << " <rules_file> <output_header_file>" << std::endl;
    //     return 1;
    // }

    std::string rulesFile = "../lex_rules.txt";
    std::string outputFile = "../Generated/DFA_Tables.hpp";

    // 创建正则表达式引擎
    Compiler::RegexEngine regexEngine;

    // 从文件加载规则
    if (!regexEngine.loadRulesFromFile(rulesFile)) {
        std::cerr << "Error: Failed to load rules from file: " << rulesFile << std::endl;
        return 1;
    }

    // 构建NFA
    std::shared_ptr<Compiler::NFA> nfa = regexEngine.buildCombinedNFA();
    if (!nfa) {
        std::cerr << "Error: Failed to build NFA" << std::endl;
        return 1;
    }

    // 将NFA转换为DFA
    std::shared_ptr<Compiler::DFA> dfa = nfa->toDFA();
    if (!dfa) {
        std::cerr << "Error: Failed to convert NFA to DFA" << std::endl;
        return 1;
    }

    // 最小化DFA
    dfa->minimize();

    // 导出DFA表到头文件
    if (!dfa->exportToHeaderFile(outputFile)) {
        std::cerr << "Error: Failed to export DFA to header file: " << outputFile << std::endl;
        return 1;
    }

    std::cout << "DFA has been successfully generated and exported to: " << outputFile << std::endl;

    return 0;
}
