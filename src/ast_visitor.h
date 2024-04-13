//
// Created by jon on 4/13/24.
//

#ifndef ELLIS_AST_VISITOR_H
#define ELLIS_AST_VISITOR_H


#include <llvm/IR/Value.h>
#include "ast.hpp"

class ASTVisitor {
public:
    virtual llvm::Value* codegen(const NumberExprAST &expr) = 0;
    virtual llvm::Value* codegen(const CharExprAST &expr) = 0;
    virtual llvm::Value* codegen(const StringExprAST &expr) = 0;
    virtual llvm::Value* codegen(const VariableExprAST &expr) = 0;
    virtual llvm::Value* codegen(const VariableDefAST &expr) = 0;
    virtual llvm::Value* codegen(const BinaryExprAST &expr) = 0;
    virtual llvm::Value* codegen(const CallExprAST &expr) = 0;
    virtual llvm::Value* codegen(const Function &expr) = 0;
    virtual llvm::Value* codegen(const IfAST &expr) = 0;

};


#endif //ELLIS_AST_VISITOR_H
