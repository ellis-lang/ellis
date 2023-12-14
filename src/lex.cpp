//
// Created by jonathan on 12/14/23.
//

#include "lex.hpp"

#include <iostream>
#include <set>

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
};

bool isoper(const char c) {
    return operator_set.count(c) > 0;
}


Token get_token_type(const std::string& s) {
    if (TOKEN_MAP.count(s))
        return TOKEN_MAP.at(s);

    if (isalpha(s[0]))
        return tok_identifier;

    if (isdigit(s[0]))
        return tok_number;

    throw std::invalid_argument("Token not recognized: " + s);
}


TokenPair lex_alphanum(std::string& source) {
    std::string alphanum = "";
    auto tp = TokenPair();
    if (source.length() > 0) {
        for (const auto c : source) {
            if (isalnum(c)) {
                alphanum += c;
            } else break;
        }
    }
    source.erase(0, alphanum.length());
    tp.second = alphanum;
    tp.first = get_token_type(alphanum);
    return tp;
}

TokenPair lex_operator(std::string& source) {
    std::string op = "";
    auto tp = TokenPair();
    if (source.length() > 0) {
        for (const auto c : source) {
            if (isoper(c)) {
                op += c;
            } else break;
        }
    }
    source.erase(0, op.length());
    tp.second = op;
    tp.first = get_token_type(op);
    return tp;
}

TokenPair lex_string_literal(std::string& source) {
    std::string str = "";
    auto tp = TokenPair();
    bool escaped = false;
    source.erase(0, 1); // remove initial double quote

    for (const auto c : source) {
        if (c == '"' && !escaped)
            break;

        if (c == '\\') {
            escaped = true;
            str += c;
        } else {
            escaped = false;
            str += c;
        }
    }
    source.erase(0, str.length() + 1);
    tp.second = str;
    tp.first = tok_string_literal;
    return tp;
}

TokenPair lex_char_literal(std::string& source) {

}

void print_token(const TokenPair& tp) {
    std::cout << "<Token: " << tp.first << ", " << tp.second << " >\n";
}


std::vector<TokenPair> lex(std::string source, bool verbose=false) {
    auto tokens = std::vector<TokenPair>();
    while (!source.empty()) {
        //  strip all whitespace
        auto ws = source.find_first_not_of(" \t\n\v\f\r");
        source.erase(0, ws);

        if (isalnum(source[0])) {
            tokens.push_back(lex_alphanum(source));
        } else if (isoper(source[0])) {
            tokens.push_back(lex_operator(source));
        } else if (source[0] == '"') {
            tokens.push_back(lex_string_literal(source));
        } else if (source[0] == '\'') {
            tokens.push_back(lex_char_literal(source));
        }
        else if (TOKEN_MAP.count(std::string(1, source[0]))) {
            std::string t(1, source[0]);
            TokenPair tp = {get_token_type(t), t};
            tokens.push_back(tp);
            source.erase(0, t.size());
        } else {
            throw std::invalid_argument("Token not recognized: " + std::string(1, source[0]));
        }
    }

    if (verbose) {
        for (auto& t : tokens) {
            print_token(t);
        }
    }

    return tokens;
}
