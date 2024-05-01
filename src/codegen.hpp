//
// Created by jon on 4/13/24.
//

#ifndef ELLIS_CODEGEN_VISITOR_H
#define ELLIS_CODEGEN_VISITOR_H


#include <map>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/IRBuilder.h>
#include "ast.hpp"

using namespace llvm;

using Code = std::variant<Value*, Function*>;

class CodeGenerator {
    Code codegen_variable_ast(const VariableExprAST& ast);
    Code codegen_function_ast(const FunctionAST& ast);
    Code codegen_number_ast(const NumberExprAST& ast);
    Code codegen_string_ast(const StringExprAST& ast);
    Code codegen_char_ast(const CharExprAST& ast);
    Code codegen_callee_ast(const CallExprAST& ast);
    Code codegen_variable_def_ast(const LetExprAST& ast);
    Code codegen_if_ast(const IfAST& ast);
    Code codegen_binary_ast(const BinaryExprAST& ast);
    Code codegen_return_ast(const ReturnAST& ast);
    Code codegen_unit_ast(const UnitExprAST& ast);
    Code codegen_prototype_ast(const PrototypeAST& ast);

    AllocaInst *CreateEntryBlockAlloca(Function *TheFunction,
                                       const std::string &VarName) {
        IRBuilder<> TmpB(&TheFunction->getEntryBlock(),
                         TheFunction->getEntryBlock().begin());
        return TmpB.CreateAlloca(Type::getDoubleTy(*context), nullptr,
                                 VarName);
    }
    
protected: /* Used by tester */
    std::unique_ptr<llvm::LLVMContext> context;
    std::unique_ptr<llvm::IRBuilder<>> builder;
    std::unique_ptr<llvm::Module> module;
    std::map<std::string, AllocaInst *> variables;
    std::map<std::string, Function*> functions;

public:
    CodeGenerator();

    Code codegen(const AST& ast) {
        return std::visit(overloaded{
                [this](const NumberExprAST& ast) { return codegen_number_ast(ast); },
                [this](const StringExprAST& ast) { return codegen_string_ast(ast); },
                [this](const CharExprAST& ast) { return codegen_char_ast(ast); },
                [this](const LetExprAST& ast) { return codegen_variable_def_ast(ast); },
                [this](const FunctionAST& ast) { return codegen_function_ast(ast); },
                [this](const BinaryExprAST& ast) { return codegen_binary_ast(ast); },
                [this](const VariableExprAST& ast) { return codegen_variable_ast(ast); },
                [this](const PrototypeAST& ast) { return codegen_prototype_ast(ast); },
                [this](const IfAST& ast) { return codegen_if_ast(ast); },
                [this](const ReturnAST& ast) { return codegen_return_ast(ast); },
                [this](const CallExprAST& ast) { return codegen_callee_ast(ast); },
                [this](const UnitExprAST& ast) { return codegen_unit_ast(ast); }
        }, ast);
    }
};

class CodegenException : public std::exception {
    std::string errorMessage;

public:
    CodegenException(std::string msg)
            : errorMessage("IR Codegen Error: " + msg){};
    const char *what() const throw() { return errorMessage.c_str(); }
};


#endif //ELLIS_CODEGEN_VISITOR_H
