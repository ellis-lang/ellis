//
// Created by jonathan on 2/15/24.
//

#ifndef CODEGEN_HPP
#define CODEGEN_HPP

#include <map>
#include "ast.hpp"
#include "llvm/ADT/APFloat.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"

using namespace llvm;

class CodeGenerator: public Visitor {

    LLVMContext& TheContext;
    IRBuilder<>& Builder;
    Module& TheModule;
    std::map<std::string, Value*>& NamedValues;
    std::vector<Value*> code;

    Value* LogErrorV(const char *Str) {
        LogError(Str);
        return nullptr;
    }
public:
    CodeGenerator(LLVMContext& context, IRBuilder<>& builder, Module& module, std::map<std::string, Value*>& namedValues)
        : TheContext(context), Builder(builder), TheModule(module), NamedValues(namedValues)  {}


    void Visit(VariableExprAST& ast) override {
        Value *V = NamedValues[ast.getName()];
        if (!V)
            LogErrorV("Unknown variable name");
        ast.setCode(V);
    }

    void Visit(NumberExprAST& ast) override {
        ast.setCode(ConstantFP::get(TheContext, APFloat(ast.getVal())));
    }

    void Visit(VariableDefAST& ast) override {

    }

    void Visit(StringExprAST& ast) override {

    }

    void Visit(CharExprAST& ast) override {

    }

    void Visit(IfAST& ast) override {

    }

    void Visit(FunctionAST& ast) override {

    }

    void Visit(BinaryExprAST& ast) override {

    }

    void Visit(CallExprAST& ast) override {

    }

    void Visit(UnitExprAST& ast) override {

    }
};

#endif //CODEGEN_HPP
