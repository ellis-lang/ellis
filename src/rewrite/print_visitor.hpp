//
// Created by jonathan on 4/27/24.
//

#ifndef ELLIS_PRINT_VISITOR_HPP
#define ELLIS_PRINT_VISITOR_HPP

#include <iostream>
#include <type_traits>
#include "abstract_syntax_tree.h"

void print(const AST& expr) {
    return std::visit(overloaded{
            [](const NumberExprAST& kind) {  },
            [](const StringExprAST& kind) {  },
            [](const CharExprAST& kind) { },
            [](const LetExprAST& kind) { /* ... */ },
            [](const FunctionAST& kind) {},
            [](const BinaryExprAST& kind) {}
    }, expr);
}

#endif //ELLIS_PRINT_VISITOR_HPP
