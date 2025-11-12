#ifndef PARSER_TABLE_HPP
#define PARSER_TABLE_HPP

#include <string>
#include <map>
#include <vector>

namespace Compiler {

// 符号类型
enum class SymbolType {
    TERMINAL,
    NON_TERMINAL,
    EPSILON
};

// 终结符映射
const std::map<std::string, int> TERMINALS = {
    {"!=", 0},
    {"(", 1},
    {")", 2},
    {"*", 3},
    {"+", 4},
    {"-", 5},
    {"/", 6},
    {";", 7},
    {"<", 8},
    {"<=", 9},
    {"=", 10},
    {"==", 11},
    {">", 12},
    {">=", 13},
    {"IDENTIFIER", 14},
    {"NUMBER", 15},
    {"else", 16},
    {"for", 17},
    {"if", 18},
    {"int", 19},
    {"read", 20},
    {"while", 21},
    {"write", 22},
    {"{", 23},
    {"}", 24},
};

// 非终结符映射
const std::map<std::string, int> NON_TERMINALS = {
    {"<additive_expr>", 0},
    {"<additive_expr_prime>", 1},
    {"<compound_stat>", 2},
    {"<declaration_list>", 3},
    {"<declaration_stat>", 4},
    {"<else_part>", 5},
    {"<expression>", 6},
    {"<expression_prime>", 7},
    {"<expression_stat>", 8},
    {"<factor>", 9},
    {"<for_stat>", 10},
    {"<if_stat>", 11},
    {"<other_stat>", 12},
    {"<program>", 13},
    {"<read_stat>", 14},
    {"<rel_op>", 15},
    {"<statement>", 16},
    {"<statement_list>", 17},
    {"<term>", 18},
    {"<term_prime>", 19},
    {"<while_stat>", 20},
    {"<write_stat>", 21},
};

// 产生式定义
struct Production {
    std::string left;
    std::vector<std::pair<std::string, SymbolType>> right;
};

const std::vector<Production> PRODUCTIONS = {
    {"<program>", {
        {"{", SymbolType::TERMINAL},
        {"<declaration_list>", SymbolType::NON_TERMINAL},
        {"<statement_list>", SymbolType::NON_TERMINAL},
        {"}", SymbolType::TERMINAL},
    }},
    {"<declaration_list>", {
        {"<declaration_stat>", SymbolType::NON_TERMINAL},
        {"<declaration_list>", SymbolType::NON_TERMINAL},
    }},
    {"<declaration_list>", {
        {"ε", SymbolType::EPSILON},
    }},
    {"<declaration_stat>", {
        {"int", SymbolType::TERMINAL},
        {"IDENTIFIER", SymbolType::TERMINAL},
        {";", SymbolType::TERMINAL},
    }},
    {"<statement_list>", {
        {"<statement>", SymbolType::NON_TERMINAL},
        {"<statement_list>", SymbolType::NON_TERMINAL},
    }},
    {"<statement_list>", {
        {"ε", SymbolType::EPSILON},
    }},
    {"<statement>", {
        {"<if_stat>", SymbolType::NON_TERMINAL},
    }},
    {"<statement>", {
        {"<other_stat>", SymbolType::NON_TERMINAL},
    }},
    {"<if_stat>", {
        {"if", SymbolType::TERMINAL},
        {"(", SymbolType::TERMINAL},
        {"<expression>", SymbolType::NON_TERMINAL},
        {")", SymbolType::TERMINAL},
        {"<statement>", SymbolType::NON_TERMINAL},
        {"<else_part>", SymbolType::NON_TERMINAL},
    }},
    {"<else_part>", {
        {"else", SymbolType::TERMINAL},
        {"<statement>", SymbolType::NON_TERMINAL},
    }},
    {"<else_part>", {
        {"ε", SymbolType::EPSILON},
    }},
    {"<other_stat>", {
        {"<while_stat>", SymbolType::NON_TERMINAL},
    }},
    {"<other_stat>", {
        {"<for_stat>", SymbolType::NON_TERMINAL},
    }},
    {"<other_stat>", {
        {"<write_stat>", SymbolType::NON_TERMINAL},
    }},
    {"<other_stat>", {
        {"<read_stat>", SymbolType::NON_TERMINAL},
    }},
    {"<other_stat>", {
        {"<compound_stat>", SymbolType::NON_TERMINAL},
    }},
    {"<other_stat>", {
        {"<expression_stat>", SymbolType::NON_TERMINAL},
    }},
    {"<while_stat>", {
        {"while", SymbolType::TERMINAL},
        {"(", SymbolType::TERMINAL},
        {"<expression>", SymbolType::NON_TERMINAL},
        {")", SymbolType::TERMINAL},
        {"<statement>", SymbolType::NON_TERMINAL},
    }},
    {"<for_stat>", {
        {"for", SymbolType::TERMINAL},
        {"(", SymbolType::TERMINAL},
        {"<expression>", SymbolType::NON_TERMINAL},
        {";", SymbolType::TERMINAL},
        {"<expression>", SymbolType::NON_TERMINAL},
        {";", SymbolType::TERMINAL},
        {"<expression>", SymbolType::NON_TERMINAL},
        {")", SymbolType::TERMINAL},
        {"<statement>", SymbolType::NON_TERMINAL},
    }},
    {"<write_stat>", {
        {"write", SymbolType::TERMINAL},
        {"<expression>", SymbolType::NON_TERMINAL},
        {";", SymbolType::TERMINAL},
    }},
    {"<read_stat>", {
        {"read", SymbolType::TERMINAL},
        {"IDENTIFIER", SymbolType::TERMINAL},
        {";", SymbolType::TERMINAL},
    }},
    {"<compound_stat>", {
        {"{", SymbolType::TERMINAL},
        {"<statement_list>", SymbolType::NON_TERMINAL},
        {"}", SymbolType::TERMINAL},
    }},
    {"<expression_stat>", {
        {"<expression>", SymbolType::NON_TERMINAL},
        {";", SymbolType::TERMINAL},
    }},
    {"<expression_stat>", {
        {";", SymbolType::TERMINAL},
    }},
    {"<expression>", {
        {"<additive_expr>", SymbolType::NON_TERMINAL},
        {"<expression_prime>", SymbolType::NON_TERMINAL},
    }},
    {"<expression_prime>", {
        {"<rel_op>", SymbolType::NON_TERMINAL},
        {"<additive_expr>", SymbolType::NON_TERMINAL},
    }},
    {"<expression_prime>", {
        {"=", SymbolType::TERMINAL},
        {"<expression>", SymbolType::NON_TERMINAL},
    }},
    {"<expression_prime>", {
        {"ε", SymbolType::EPSILON},
    }},
    {"<rel_op>", {
        {">", SymbolType::TERMINAL},
    }},
    {"<rel_op>", {
        {"<", SymbolType::TERMINAL},
    }},
    {"<rel_op>", {
        {">=", SymbolType::TERMINAL},
    }},
    {"<rel_op>", {
        {"<=", SymbolType::TERMINAL},
    }},
    {"<rel_op>", {
        {"==", SymbolType::TERMINAL},
    }},
    {"<rel_op>", {
        {"!=", SymbolType::TERMINAL},
    }},
    {"<additive_expr>", {
        {"<term>", SymbolType::NON_TERMINAL},
        {"<additive_expr_prime>", SymbolType::NON_TERMINAL},
    }},
    {"<additive_expr_prime>", {
        {"+", SymbolType::TERMINAL},
        {"<term>", SymbolType::NON_TERMINAL},
        {"<additive_expr_prime>", SymbolType::NON_TERMINAL},
    }},
    {"<additive_expr_prime>", {
        {"-", SymbolType::TERMINAL},
        {"<term>", SymbolType::NON_TERMINAL},
        {"<additive_expr_prime>", SymbolType::NON_TERMINAL},
    }},
    {"<additive_expr_prime>", {
        {"ε", SymbolType::EPSILON},
    }},
    {"<term>", {
        {"<factor>", SymbolType::NON_TERMINAL},
        {"<term_prime>", SymbolType::NON_TERMINAL},
    }},
    {"<term_prime>", {
        {"*", SymbolType::TERMINAL},
        {"<factor>", SymbolType::NON_TERMINAL},
        {"<term_prime>", SymbolType::NON_TERMINAL},
    }},
    {"<term_prime>", {
        {"/", SymbolType::TERMINAL},
        {"<factor>", SymbolType::NON_TERMINAL},
        {"<term_prime>", SymbolType::NON_TERMINAL},
    }},
    {"<term_prime>", {
        {"ε", SymbolType::EPSILON},
    }},
    {"<factor>", {
        {"(", SymbolType::TERMINAL},
        {"<expression>", SymbolType::NON_TERMINAL},
        {")", SymbolType::TERMINAL},
    }},
    {"<factor>", {
        {"IDENTIFIER", SymbolType::TERMINAL},
    }},
    {"<factor>", {
        {"NUMBER", SymbolType::TERMINAL},
    }},
};

// LL(1)分析表 [非终结符索引][终结符索引] = 产生式索引(-1表示空)
const std::map<std::pair<int, int>, int> PARSING_TABLE = {
    {{0, 1}, 34},
    {{0, 14}, 34},
    {{0, 15}, 34},
    {{1, 0}, 37},
    {{1, 2}, 37},
    {{1, 4}, 35},
    {{1, 5}, 36},
    {{1, 7}, 37},
    {{1, 8}, 37},
    {{1, 9}, 37},
    {{1, 10}, 37},
    {{1, 11}, 37},
    {{1, 12}, 37},
    {{1, 13}, 37},
    {{2, 23}, 21},
    {{3, 1}, 2},
    {{3, 7}, 2},
    {{3, 14}, 2},
    {{3, 15}, 2},
    {{3, 17}, 2},
    {{3, 18}, 2},
    {{3, 19}, 1},
    {{3, 20}, 2},
    {{3, 21}, 2},
    {{3, 22}, 2},
    {{3, 23}, 2},
    {{3, 24}, 2},
    {{4, 19}, 3},
    {{5, 1}, 10},
    {{5, 7}, 10},
    {{5, 14}, 10},
    {{5, 15}, 10},
    {{5, 16}, 9},
    {{5, 17}, 10},
    {{5, 18}, 10},
    {{5, 20}, 10},
    {{5, 21}, 10},
    {{5, 22}, 10},
    {{5, 23}, 10},
    {{5, 24}, 10},
    {{6, 1}, 24},
    {{6, 14}, 24},
    {{6, 15}, 24},
    {{7, 0}, 25},
    {{7, 2}, 27},
    {{7, 7}, 27},
    {{7, 8}, 25},
    {{7, 9}, 25},
    {{7, 10}, 26},
    {{7, 11}, 25},
    {{7, 12}, 25},
    {{7, 13}, 25},
    {{8, 1}, 22},
    {{8, 7}, 23},
    {{8, 14}, 22},
    {{8, 15}, 22},
    {{9, 1}, 42},
    {{9, 14}, 43},
    {{9, 15}, 44},
    {{10, 17}, 18},
    {{11, 18}, 8},
    {{12, 1}, 16},
    {{12, 7}, 16},
    {{12, 14}, 16},
    {{12, 15}, 16},
    {{12, 17}, 12},
    {{12, 20}, 14},
    {{12, 21}, 11},
    {{12, 22}, 13},
    {{12, 23}, 15},
    {{13, 23}, 0},
    {{14, 20}, 20},
    {{15, 0}, 33},
    {{15, 8}, 29},
    {{15, 9}, 31},
    {{15, 11}, 32},
    {{15, 12}, 28},
    {{15, 13}, 30},
    {{16, 1}, 7},
    {{16, 7}, 7},
    {{16, 14}, 7},
    {{16, 15}, 7},
    {{16, 17}, 7},
    {{16, 18}, 6},
    {{16, 20}, 7},
    {{16, 21}, 7},
    {{16, 22}, 7},
    {{16, 23}, 7},
    {{17, 1}, 4},
    {{17, 7}, 4},
    {{17, 14}, 4},
    {{17, 15}, 4},
    {{17, 17}, 4},
    {{17, 18}, 4},
    {{17, 20}, 4},
    {{17, 21}, 4},
    {{17, 22}, 4},
    {{17, 23}, 4},
    {{17, 24}, 5},
    {{18, 1}, 38},
    {{18, 14}, 38},
    {{18, 15}, 38},
    {{19, 0}, 41},
    {{19, 2}, 41},
    {{19, 3}, 39},
    {{19, 4}, 41},
    {{19, 5}, 41},
    {{19, 6}, 40},
    {{19, 7}, 41},
    {{19, 8}, 41},
    {{19, 9}, 41},
    {{19, 10}, 41},
    {{19, 11}, 41},
    {{19, 12}, 41},
    {{19, 13}, 41},
    {{20, 21}, 17},
    {{21, 22}, 19},
};

} // namespace Compiler

#endif // PARSER_TABLE_HPP
