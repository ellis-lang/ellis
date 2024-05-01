//
// Created by jonathan on 4/27/24.
//

#include "ast.hpp"
#include "print.hpp"

void PrintVisitor::operator()(NumberExprAST &ast) {
    stream << "Number(" << ast.val << ")";
}

void PrintVisitor::operator()(CallExprAST &ast) {

}

void PrintVisitor::operator()(CharExprAST &ast) {

}

void PrintVisitor::operator()(StringExprAST &) {

}

void PrintVisitor::operator()(UnitExprAST &) {

}

void PrintVisitor::operator()(VariableExprAST &) {

}

void PrintVisitor::operator()(BinaryExprAST &) {

}

void PrintVisitor::operator()(LetExprAST &) {

}

void PrintVisitor::operator()(FunctionAST &) {

}

void PrintVisitor::operator()(PrototypeAST &) {

}

void PrintVisitor::operator()(ReturnAST &) {

}

void PrintVisitor::operator()(IfAST &) {

}
