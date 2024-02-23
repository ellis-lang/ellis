//
// Created by jonathan on 12/14/23.
//

#ifndef ELLIS_LEX_HPP
#define ELLIS_LEX_HPP
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

enum Token {
    tok_eof = -1,
    tok_let = 1,
    tok_if = 2,
    tok_for = 3,
    tok_while = 4,
    tok_else = 5,
    tok_end = 6,
    tok_then = 7,
    tok_in = 8,
    tok_return = 9,
    tok_and = 41,
    tok_or = 42,
    tok_not = 43,
    tok_log_op = 44,

    tok_lparen = 20,
    tok_rparen = 21,
    tok_semicolon = 22,
    tok_colon = 23,
    tok_comma = 24,
    tok_lbracket = 25,
    tok_rbracket = 26,
    tok_unit = 27,

    tok_exponent = 17,

    tok_double_quote = 30,
    tok_single_quote = 31,
    tok_string_literal = 32,
    tok_char_literal = 33,
    tok_operator = 34,

    tok_identifier = -4,
    tok_number = -5
};

const std::map<std::string, Token> TOKEN_MAP = {
    {"let",tok_let},
    {"if",tok_if},
    {"else",tok_else},
    {"for",tok_for},
    {"while",tok_while},
    {"end", tok_end},
    {"(", tok_lparen},
    {")", tok_rparen},
    {";", tok_semicolon},
    {"\"", tok_double_quote},
    {"'", tok_single_quote},
    {"then", tok_then},
    {"in", tok_in},
    {"or", tok_log_op},
    {"and", tok_log_op},
    {"not", tok_not},
    {"return", tok_return}
};

const std::map<Token, std::string> TOKEN_STRINGS = {
    {tok_let, "LET"},
    {tok_if, "IF"},
    {tok_else, "ELSE"},
    {tok_for, "FOR"},
    {tok_while, "WHILE"},
    {tok_end, "END"},
    {tok_lparen, "LPAREN"},
    {tok_rparen, "RPAREN"},
    {tok_semicolon, "SEMICOLON"},
    {tok_colon, "COLON"},
    {tok_double_quote, "\""},
    {tok_single_quote, "'"},
    {tok_identifier, "IDENTIFIER"},
    {tok_number, "NUMBER"},
    {tok_string_literal, "STRING_LITERAL"},
    {tok_char_literal, "CHAR_LITERAL"},
    {tok_then, "THEN"},
    {tok_in, "IN"},
    {tok_operator, "OP"},
    {tok_return, "RETURN"}
};

const std::set operator_set = {
    '+',
    '-',
    '/',
    '*',
    '%',
    '^',
    '&',
    '!',
    '=',
    '|',
    '<',
    '>',
};

const std::set infix_operators = {
    "+",
    "-",
    "/",
    "*",
    "^",
    "%",
    "&",
    "|",
    ">",
    "<",

    "+=",
    "-=",
    "<=",
    ">=",
    "==",
    "!=",
    "^=",
    "%=",
    "^=",
    "&=",
    "|="
};


typedef std::pair<Token, std::string> TokenPair;

Token get_token_type(const std::string& s);
std::vector<TokenPair> lex(std::string source, bool verbose);


#endif //ELLIS_LEX_HPP
