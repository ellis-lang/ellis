//
// Created by jonathan on 4/27/24.
//

#ifndef ELLIS_PRINT_HPP
#define ELLIS_PRINT_HPP

#include <iostream>
#include <type_traits>
#include "ast.hpp"

void print(const AST& expr, std::ostream& stream) {

    std::visit(overloaded{
        [&stream](const NumberExprAST& ast) { stream << "Number(" << ast.val << ")"; },
        [&stream](const StringExprAST& ast) { stream << "String(" << ast.val << ")"; },
        [&stream](const CharExprAST& ast) { stream << "Char(" << ast.val << ")"; },
        [&stream](const LetExprAST& ast) {
            stream << "VariableDef(" << ast.name << " = ";
            print(*ast.val,stream);
            stream << ")";
        },
        [&stream](const FunctionAST& ast) {
            stream << "Function ";
            print(*ast.proto, stream);
            stream << "\n";
            for (const auto &n : ast.body) {
                stream << "\t";
                print(*n, stream);
                stream << "\n";
            }
        },
        [&stream](const BinaryExprAST& ast) {
            stream << ast.op << "(";
            print(*ast.lhs, stream);
            stream << " , ";
            print(*ast.rhs, stream);
            stream << ")";
        },
        [&stream](const VariableExprAST& ast) { stream << "Variable(" << ast.name << ")"; },
        [&stream](const PrototypeAST& ast) {
            stream << ast.name << "( ";
            for (const auto& arg: ast.args)
                stream << arg << " ";
            stream << ")";
        },
        [&stream](const IfAST& ast) {
            stream << "If(  ";
            print(*ast.cond, stream);
            stream << ") then \n";
            for (const auto &n : ast.body_on_true) {
                stream << "\t";
                print(*n, stream);
                stream << "\n";
            }
            stream << " else \n";
            for (const auto &n : ast.body_on_false) {
                stream << "\t";
                print(*n, stream);
                stream << "\n";
            }
        },
        [&stream](const ReturnAST& ast) {
            stream << "Return(";
            if (ast.val != nullptr) {
                print(*ast.val, stream);
            }
            stream << ")";
        },
        [&stream](const CallExprAST& ast) {
            stream << ast.callee << "( ";
            for (const auto& arg : ast.args) {
                print(*arg, stream);
                stream << " ";
            }
            stream << ")";
        },
        [&stream](const UnitExprAST& ast) { stream << "Unit()"; }
    }, expr);
    stream << "\n";
}

#endif //ELLIS_PRINT_HPP
