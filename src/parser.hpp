//
// Created by jonathan on 2/12/24.
//

#ifndef PARSER_HPP
#define PARSER_HPP

#include "ast.hpp"
#include "lex.hpp"

std::vector<std::unique_ptr<AST>> parse(std::vector<TokenPair>& tokens);

class ParsingException : public std::exception {
    std::string message;
public:
    explicit ParsingException(const std::string& msg) : message(msg) {}
    const char* what () const noexcept override {
        return message.c_str();
    }
};

#endif //PARSER_HPP
