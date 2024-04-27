//
// Created by jon on 4/13/24.
//

#ifndef ELLIS_CODEGEN_VISITOR_H
#define ELLIS_CODEGEN_VISITOR_H


#include <map>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/IRBuilder.h>
#include "ast_visitor.h"

using namespace llvm;

using Code = std::variant<Value*, Function*>;

class CodegenVisitor: public Visitor {

protected: /* Used by tester */
    std::unique_ptr<llvm::LLVMContext> context;
    std::unique_ptr<llvm::IRBuilder<>> builder;
    std::unique_ptr<llvm::Module> module;
    std::map<std::string, llvm::AllocaInst *> varEnv;

public:
    CodegenVisitor();
    void operator()(NumberExprAST&);
    void operator()(CallExprAST&);
    void operator()(CharExprAST&);
    void operator()(StringExprAST&);
    void operator()(UnitExprAST&);
    void operator()(VariableExprAST&);
    void operator()(LetExprAST&);
    void operator()(BinaryExprAST&);
    void operator()(FunctionAST&);
    void operator()(PrototypeAST&);
    void operator()(ReturnAST&);
    void operator()(IfAST&);
};

class CodegenException : public std::exception {
    std::string errorMessage;

public:
    CodegenException(std::string msg)
            : errorMessage("IR Codegen Error: " + msg){};
    const char *what() const throw() { return errorMessage.c_str(); }
};


#endif //ELLIS_CODEGEN_VISITOR_H
