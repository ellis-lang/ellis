//
// Created by jon on 4/13/24.
//

#ifndef ELLIS_CODEGEN_VISITOR_H
#define ELLIS_CODEGEN_VISITOR_H


#include <map>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/IRBuilder.h>
#include "ast_visitor.h"

class CodegenVisitor: public ASTVisitor{

protected: /* Used by tester */
    std::unique_ptr<llvm::LLVMContext> context;
    std::unique_ptr<llvm::IRBuilder<>> builder;
    std::unique_ptr<llvm::Module> module;
    std::map<std::string, llvm::AllocaInst *> varEnv;

public:
    CodegenVisitor();
    llvm::Value* codegen(const NumberExprAST &expr) override;
    llvm::Value* codegen(const CharExprAST &expr) override;
    llvm::Value* codegen(const StringExprAST &expr) override;
    llvm::Value* codegen(const VariableExprAST &expr) override;
    llvm::Value* codegen(const VariableDefAST &expr) override;
    llvm::Value* codegen(const BinaryExprAST &expr) override;
    llvm::Value* codegen(const CallExprAST &expr) override;
    llvm::Value* codegen(const Function &expr) override;
    llvm::Value* codegen(const IfAST &expr) override;

    void dumpLLVMIR();
};

class CodegenException : public std::exception {
    std::string errorMessage;

public:
    CodegenException(std::string msg)
            : errorMessage("IR Codegen Error: " + msg){};
    const char *what() const throw() { return errorMessage.c_str(); }
};


#endif //ELLIS_CODEGEN_VISITOR_H
