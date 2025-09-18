#include "TESTscan.hpp"

// 类方法实现
void LexicalAnalyzer::lexicalAnalysis(const std::vector<std::string> &lines) {
    bool isOver = false, isCommenting = false;
    // 清空之前的符号表
    SymbolTable.clear();
    std::cout << "Analyzing" << std::endl;
    size_t potentialUnclosedComments = 0;

    for (size_t lineIndex = 0; lineIndex < lines.size(); lineIndex++) {
        std::string line = lines[lineIndex];
        size_t lineSize = line.size();

        for (size_t i = 0; i < lineSize; i++) {
            if (!isOver) {
                char ch = line[i];

                //检查注释状态
                if (isCommenting) {
                    isComment(lineIndex, i, lineSize, lines.size(), line, potentialUnclosedComments, isCommenting, isOver);
                    if (isOver)
                        break;
                    if (isCommenting) {
                        continue;
                    }
                }
                // 开始判断字符类型
                if (isLetter(ch)) {
                    std::string token;
                    while (i < lineSize && (isLetter(line[i]) || isDigit(line[i]))) {
                        token += line[i];
                        i++;
                    }
                    i--; // 调整位置

                    // 使用哈希表O(1)查找替代find
                    if (keywords.find(token) != keywords.end()) {
                        // 关键词类型与token相同
                        SymbolTable.push_back({token, token});
                    }
                    else {
                        SymbolTable.push_back({"ID", token});
                    }
                }
                else if (isDigit(ch)) {
                    std::string token;
                    while (i < lineSize && isDigit(line[i])) {
                        token += line[i];
                        i++;
                    }
                    i--; // 调整位置
                    SymbolTable.push_back({"NUM", token});
                }
                else if (isSingleword(ch)) {
                    // 检查是否为注释
                    if (ch == '/' && i + 1 < lineSize && line[i + 1] == '*') {
                        i = i + 2; // 跳过 /*
                        isCommenting = true;
                        potentialUnclosedComments = lineIndex;
                        isComment(lineIndex, i, lineSize, lines.size(), line, potentialUnclosedComments, isCommenting, isOver);
                        if (isOver)
                            break;
                        if (isCommenting) {
                            continue;
                        }
                    }
                    else {
                        std::string token(1, ch);
                        SymbolTable.push_back({token, token});
                    }
                }
                else if (isPotentialDouble(ch)) {
                    if (i + 1 < lineSize) {
                        std::string potential = std::string(1, ch) + std::string(1, line[i + 1]);
                        if (isDoubleword(potential)) {
                            SymbolTable.push_back({potential, potential});
                            i++; // 跳过下一个字符
                        }
                        else {
                            std::string token(1, ch);
                            SymbolTable.push_back({token, token});
                        }
                    }
                }
                else if (isBlank(ch)) {
                    continue; // 忽略空白字符
                }
                else {
                    std::cerr << "Error: " << "On Line " << lineIndex + 1 << " :Unrecognized character '" << ch << "'" << std::endl;
                    continue;
                }
            }
            else {
                break;
            }
        }
    }
}

const std::vector<std::pair<std::string, std::string>>& LexicalAnalyzer::getSymbolTable() const {
    return SymbolTable;
}

void LexicalAnalyzer::clearSymbolTable() {
    SymbolTable.clear();
}

void LexicalAnalyzer::printSymbolTable() const {
    std::cout << "Symbol Table:\n";
    for (const auto &entry : SymbolTable) {
        std::cout << entry.first << "\t" << entry.second << std::endl;
    }
}

bool LexicalAnalyzer::isBlank(const char ch) {
    if (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r') {
        return true;
    }
    return false;
}

bool LexicalAnalyzer::isLetter(const char ch) {
    if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_') {
        return true;
    }
    return false;
}

bool LexicalAnalyzer::isDigit(const char ch) {
    if (ch >= '0' && ch <= '9') {
        return true;
    }
    return false;
}

bool LexicalAnalyzer::isSingleword(const char ch) {
    return singlewords.find(ch) != singlewords.end();
}

bool LexicalAnalyzer::isPotentialDouble(const char ch) {
    return potentialDouble.find(ch) != potentialDouble.end();
}

bool LexicalAnalyzer::isDoubleword(const std::string & str) {
    return doublewords.find(str) != doublewords.end();
}

void LexicalAnalyzer::isComment(const size_t &lineIndex, size_t &i, const size_t &lineSize, const size_t &linesCount,
    const std::string &line, const size_t &potentialUnclosedComments, bool &isCommenting, bool &isOver) {
    while (line[i] != '*' || line[i + 1] != '/') {
        i++;
        if (i >= lineSize) {
            if (lineIndex == linesCount - 1) {
                isOver = true;
                std::cerr << "Error: Comment not closed on line " << potentialUnclosedComments + 1 << std::endl;
                break;
            }
            else {
                // 继续到下一行
                return;
            }
        }
        // 如果找到了 */
        if (line[i] == '*' && line[i + 1] == '/') {
            i = i + 1; // 跳过 */
            isCommenting = false; // 结束注释
            return;
        }
    }
    return;
}

int doScan(const std::string & filename) {
    std::cout << "[scan] lexical analysis start\n";
    // std::istream *in = &std::cin;
    std::ifstream infile;
    if (!filename.empty()) {
        infile.open(filename);
        if (!infile) {
            std::cerr << "Could not open file: " << filename << "\n";
            return 1;
        }
        // in = &infile;
        std::cout << "Reading from file: " << filename << "\n";
    }
    else {
        std::cerr << "Still no input file" << std::endl;
        return 1;
    }

    std::string line;
    std::vector<std::string> lines;
    while (std::getline(infile, line)) {
        lines.push_back(line);
    }

    // 创建词法分析器实例并进行分析
    LexicalAnalyzer analyzer;
    analyzer.lexicalAnalysis(lines);
    analyzer.printSymbolTable();

    std::cout << "[scan] lexical analysis end" << std::endl;
    if (infile.is_open()) {
        infile.close();
    }

    return 0;
}