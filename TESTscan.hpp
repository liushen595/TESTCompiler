#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_set>
// #include <unordered_map>
// #include <cctype>

class LexicalAnalyzer {
private:
    // 使用哈希表存储关键字，便于O(1)时间复杂度查找
    std::unordered_set<std::string> keywords = {
        "if", "else", "for", "while", "do", "int", "read", "write"
    };

    // 使用哈希表存储单字符符号，便于O(1)查找
    std::unordered_set<char> singlewords = {'+','-','*','/','(',')','{','}',';',',',':'};

    // 潜在的双字符符号的第一个字符
    std::unordered_set<char> potentialDouble = {'<','>','=','!'};

    // 使用哈希表存储双字符符号
    std::unordered_set<std::string> doublewords = {"<=",">=","==","!="};

    // 符号表，保存词法分析结果
    std::vector<std::pair<std::string, std::string>> SymbolTable; // pair<type, token>

    // 私有辅助函数
    bool isBlank(const char ch);
    bool isLetter(const char ch);
    bool isDigit(const char ch);
    bool isSingleword(const char ch);
    bool isPotentialDouble(const char ch);
    bool isDoubleword(const std::string &str);
    void isComment(const size_t &lineIndex, size_t &i, const size_t &lineSize, const size_t &linesCount,
        const std::string &line, const size_t &potentialUnclosedComments, bool &isCommenting, bool &isOver);

public:
    // 构造函数
    LexicalAnalyzer() = default;

    // 主要的词法分析函数
    void lexicalAnalysis(const std::vector<std::string> &lines);

    // 获取符号表
    const std::vector<std::pair<std::string, std::string>>& getSymbolTable() const;

    // 清空符号表
    void clearSymbolTable();

    // 打印符号表
    void printSymbolTable() const;
};

int doScan(const std::string & filename = "");