//
// Created by jon on 4/13/24.
//

#ifndef ELLIS_ABSTRACT_SYNTAX_TREE_H
#define ELLIS_ABSTRACT_SYNTAX_TREE_H

#include <iostream>
#include <variant>
#include <memory>
#include <vector>


struct NumberExprAST;
struct CharExprAST;
struct StringExprAST;
struct UnitExprAST;
struct VariableExprAST;
struct LetExprAST;
struct BinaryExprAST;
struct FunctionAST;
struct CallExprAST;
struct PrototypeAST;
struct ReturnAST;
struct IfAST;


using AST = std::variant<NumberExprAST, CallExprAST, CharExprAST, StringExprAST,
        UnitExprAST, VariableExprAST, LetExprAST, BinaryExprAST, FunctionAST, PrototypeAST,
        ReturnAST, IfAST>;


#endif //ELLIS_ABSTRACT_SYNTAX_TREE_H
