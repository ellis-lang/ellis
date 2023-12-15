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

    tok_lparen = 20,
    tok_rparen = 21,
    tok_semicolon = 22,
    tok_colon = 23,
    tok_comma = 24,
    tok_lbracket = 25,
    tok_rbracket = 26,

    tok_equal = 10,
    tok_aster = 11,
    tok_percent = 12,
    tok_plus = 13,
    tok_minus = 14,
    tok_greater_than = 15,
    tok_less_than = 16,

    tok_double_quote = 30,
    tok_single_quote = 31,
    tok_string_literal = 32,
    tok_char_literal = 33,

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
    {"+", tok_plus},
    {"-", tok_minus},
    {"*", tok_aster},
    {"=", tok_equal},
    {"\"", tok_double_quote},
    {"'", tok_single_quote},
    {">", tok_greater_than},
    {"<", tok_less_than},
    {"then", tok_then},
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
    {tok_plus, "PLUS"},
    {tok_minus, "MINUS"},
    {tok_aster, "ASTERISK"},
    {tok_equal, "EQUAL"},
    {tok_double_quote, "\""},
    {tok_single_quote, "'"},
    {tok_identifier, "IDENTIFIER"},
    {tok_number, "NUMBER"},
    {tok_string_literal, "STRING_LITERAL"},
    {tok_char_literal, "CHAR_LITERAL"},
    {tok_greater_than, "GREATER_THAN"},
    {tok_less_than, "LESS_THAN"},
    {tok_then, "THEN"}
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


typedef std::pair<Token, std::string> TokenPair;

Token get_token_type(const std::string& s);
std::vector<TokenPair> lex(std::string source, bool verbose);


#endif //ELLIS_LEX_HPP
