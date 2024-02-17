//
// Created by jonathan on 12/15/23.
//

#include "ast.hpp"

void NumberExprAST::Accept(Visitor& v) {
    v.Visit(*this);
}

void CharExprAST::Accept(Visitor& v) {
    v.Visit(*this);
}

void StringExprAST::Accept(Visitor& v) {
    v.Visit(*this);
}

void FunctionAST::Accept(Visitor& v) {
    v.Visit(*this);
}

void IfAST::Accept(Visitor& v) {
    v.Visit(*this);
}

void VariableDefAST::Accept(Visitor& v) {
    v.Visit(*this);
}

void VariableExprAST::Accept(Visitor& v) {
    v.Visit(*this);
}

void BinaryExprAST::Accept(Visitor& v) {
    v.Visit(*this);
}

void CallExprAST::Accept(Visitor& v) {
    v.Visit(*this);
}

void UnitExprAST::Accept(Visitor& v) {
    v.Visit(*this);
}
