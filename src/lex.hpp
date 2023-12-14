//
// Created by jonathan on 12/14/23.
//

#ifndef ELLIS_LEX_HPP
#define ELLIS_LEX_HPP
#include <map>
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

    tok_double_quote = 30,
    tok_single_quote = 31,
    tok_string_literal = 32,

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
    {"'", tok_single_quote}
};


typedef std::pair<Token, std::string> TokenPair;

Token get_token_type(const std::string& s);
std::vector<TokenPair> lex(std::string source, bool verbose);
TokenPair lex_alphanum(std::string& source);


#endif //ELLIS_LEX_HPP
