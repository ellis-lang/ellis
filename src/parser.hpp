//
// Created by jonathan on 2/12/24.
//

#ifndef PARSER_HPP
#define PARSER_HPP

#include "ast.hpp"
#include "lex.hpp"

std::vector<std::unique_ptr<AST>> parse(std::vector<TokenPair>& tokens);

#endif //PARSER_HPP
