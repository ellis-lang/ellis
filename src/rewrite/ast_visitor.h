//
// Created by jon on 4/13/24.
//

#ifndef ELLIS_AST_VISITOR_H
#define ELLIS_AST_VISITOR_H


#include <llvm/IR/Value.h>
#include "abstract_syntax_tree.h"

class Visitor {
public:
    virtual void operator()(NumberExprAST&) = 0;
    virtual void operator()(CallExprAST&) = 0;
    virtual void operator()(CharExprAST&) = 0;
    virtual void operator()(StringExprAST&) = 0;
    virtual void operator()(UnitExprAST&) = 0;
    virtual void operator()(VariableExprAST&) = 0;
    virtual void operator()(LetExprAST&) = 0;
    virtual void operator()(BinaryExprAST&) = 0;
    virtual void operator()(FunctionAST&) = 0;
    virtual void operator()(PrototypeAST&) = 0;
    virtual void operator()(ReturnAST&) = 0;
    virtual void operator()(IfAST&) = 0;
};


#endif //ELLIS_AST_VISITOR_H
