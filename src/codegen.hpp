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

class CodeGenerationException : public std::exception {
    std::string message;
public:
    explicit CodeGenerationException(const std::string& msg) : message(msg) {}
    const char* what () const noexcept override {
        return message.c_str();
    }
};

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

    void Visit(ExprAST& ast) {

    }

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
        Function *CalleeF = TheModule.getFunction(ast.getCallee());
        if (!CalleeF)
            throw CodeGenerationException("Unknown function referenced");

        auto Args = ast.getArgs();
        auto a = ast.Args;
        // If argument mismatch error.
        if (CalleeF->arg_size() != Args.size())
            throw CodeGenerationException("Incorrect # arguments passed");

        std::vector<Value *> ArgsV;
        for (auto & Arg : Args) {
            Arg->Accept(*this);
            ArgsV.push_back(Arg->getCode());
            if (!ArgsV.back())
                return nullptr;
        }

        ast.setCode(Builder.CreateCall(CalleeF, ArgsV, "calltmp"));
    }

    void Visit(UnitExprAST& ast) override {

    }
};

#endif //CODEGEN_HPP
