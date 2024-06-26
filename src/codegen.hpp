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
#include "llvm/IR/Verifier.h"
#include <iostream>

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
    std::vector<Value*> code;

    Value* LogErrorV(const char *Str) {
        LogError(Str);
        return nullptr;
    }
public:
    CodeGenerator(LLVMContext& context, IRBuilder<>& builder, Module& module, std::map<std::string, AllocaInst*>* namedValues)
        : TheContext(context), Builder(builder), TheModule(module), NamedValues(namedValues)  {}

        void Visit(ExprAST& ast) {}

    std::map<std::string, AllocaInst*>* NamedValues;

    AllocaInst *CreateEntryBlockAlloca(Function *TheFunction,
                                              const std::string &VarName) {
        IRBuilder<> TmpB(&TheFunction->getEntryBlock(),
                         TheFunction->getEntryBlock().begin());
        return TmpB.CreateAlloca(Type::getDoubleTy(TheContext), nullptr,
                                 VarName);
    }

    void Visit(VariableExprAST& ast) override {
        AllocaInst *V = (*NamedValues)[ast.getName()];
        if (!V) {
            auto v = TheModule.getGlobalVariable(ast.getName());
            if (v) {
                ast.setCode(v);
            } else
                LogErrorV("Unknown variable name");
        }
        else {
            auto c = Builder.CreateLoad(V->getAllocatedType(), V, ast.getName().c_str());
            if (c->getType()->isPointerTy()) {
                std::cout << "load created successfully";
            } else {
                std::cout << "could not create load";
            }
            ast.setCode(V);
        }
    }

    void Visit(NumberExprAST& ast) override {
        ast.setCode(ConstantFP::get(TheContext, APFloat(ast.getVal())));
    }

    void Visit(VariableDefAST& ast) override {
        ast.getValue().Accept(*this);
        auto c = ast.getValue().getCode();
        auto var = (*NamedValues)[ast.getName()];
        if (!var) {
            llvm::Function *parentFunction = Builder.GetInsertBlock()->getParent();
            if (!parentFunction)
                std::cout << "couldnt get parent function";
            llvm::IRBuilder<> TmpBuilder(&(parentFunction->getEntryBlock()),
                                         parentFunction->getEntryBlock().begin());
            llvm::AllocaInst *v = TmpBuilder.CreateAlloca(c.v->getType(), nullptr,
                                                            llvm::Twine(ast.getName()));

            Builder.CreateStore(c.v, v);
            (*NamedValues)[ast.getName()] = v;
            ast.setCode(c.v);

        } else {
            LogErrorV("Variable already declared");
        }
        //std::cout << "done\n";
    }

    void Visit(StringExprAST& ast) override {

    }

    void Visit(CharExprAST& ast) override {

    }

    void Visit(IfAST& ast) override {

    }

    void Visit(PrototypeAST& ast) override {
        std::vector<Type *> Doubles(ast.getArgs().size(), Type::getDoubleTy(TheContext));
        FunctionType *FT =
                FunctionType::get(Type::getDoubleTy(TheContext), Doubles, false);

        Function *F =
                Function::Create(FT, Function::ExternalLinkage, ast.getName(), TheModule);

        // Set names for all arguments.
        unsigned Idx = 0;
        for (auto &Arg : F->args())
            Arg.setName(ast.getArgs()[Idx++]);

        ast.setCode(F);
    }

    void Visit(FunctionAST& ast) override {
        // this allows functions to be redefined
        auto Args = ast.getProto().getArgs();
        std::vector<Type*> Doubles(Args.size(),
                                   Type::getDoubleTy(TheContext));
        FunctionType *FT =
                FunctionType::get(Type::getDoubleTy(TheContext), Doubles, false);

        Function *F =
                Function::Create(FT, Function::ExternalLinkage, ast.getProto().getName(), TheModule);

        unsigned Idx = 0;
        for (auto &Arg : F->args())
            Arg.setName(Args[Idx++]);

        BasicBlock *BB = BasicBlock::Create(TheContext, "entry", F);
        Builder.SetInsertPoint(BB);

        // Record the function arguments in the NamedValues map.
        //(*NamedValues).clear();
        //auto tempNamedValues = *NamedValues;
        for (auto &Arg : F->args()) {
            AllocaInst *Alloca = CreateEntryBlockAlloca(F, Arg.getName().str());

            // Store the initial value into the alloca.
            Builder.CreateStore(&Arg, Alloca);

            // Add arguments to variable symbol table.
            (*NamedValues)[std::string(Arg.getName())] = Alloca;
        }

        for (auto& expr : ast.getBody()) {
            expr->Accept(*this);
        }

        Value* RetVal = ast.getBody().back()->getCode().v;
        Builder.CreateRet(RetVal);

        verifyFunction(*F);
        ast.setCode(F);
        // doesnt handle return values
    }

    void Visit(BinaryExprAST& ast) override {
        ast.getLHS().Accept(*this);
        ast.getRHS().Accept(*this);
        auto op = ast.getOp();

        if (op == "+")
            ast.setCode(Builder.CreateFAdd(ast.getLHS().getCode().v,
                                           ast.getRHS().getCode().v, "addtmp"));
        else if (op == "-")
            ast.setCode(Builder.CreateFSub(ast.getLHS().getCode().v,
                                           ast.getRHS().getCode().v, "subtmp"));
        else if (op == "*")
            ast.setCode(Builder.CreateFMul(ast.getLHS().getCode().v,
                                           ast.getRHS().getCode().v, "multmp"));
        else if (op == "<")
            ast.setCode(Builder.CreateUIToFP(ast.getLHS().getCode().v,
                                             Type::getDoubleTy(TheContext), "booltmp"));
        else
            throw CodeGenerationException("Infix operator not implemented: " + op);
    }

    void Visit(CallExprAST& ast) override {
        Function *CalleeF = TheModule.getFunction(ast.getCallee());
        if (!CalleeF)
            throw CodeGenerationException("Unknown function referenced");

        auto& Args = ast.getArgs();
        // If argument mismatch error.
        if (CalleeF->arg_size() != Args.size())
            throw CodeGenerationException("Incorrect # arguments passed to function: " + ast.getCallee());

        std::vector<Value *> ArgsV;
        for (auto & Arg : Args) {
            Arg->Accept(*this);
            ArgsV.push_back(Arg->getCode().v);
        }

        ast.setCode(Builder.CreateCall(CalleeF, ArgsV, "calltmp"));
    }

    void Visit(UnitExprAST& ast) override {

    }

    void Visit(ReturnAST& ast) override {
        ast.getRetExpr().Accept(*this);
        Value* retVal = ast.getCode().v;
        Builder.CreateRet(retVal);
    }
};

#endif //CODEGEN_HPP
