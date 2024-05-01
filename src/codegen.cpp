//
// Created by jon on 4/13/24.
//

#include "codegen.hpp"

#include <memory>
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"
#include "llvm/Transforms/Utils.h"
#include "llvm/ADT/APFloat.h"
#include "llvm/IR/Constants.h"



CodeGenerator::CodeGenerator() {
    context = std::make_unique<llvm::LLVMContext>();
    builder = std::make_unique<llvm::IRBuilder<>>(*context);
    module = std::make_unique<llvm::Module>("Module", *context);
    variables = std::map<std::string, AllocaInst*>();
    functions = std::map<std::string, Function*>();
}

Code CodeGenerator::codegen_number_ast(const NumberExprAST &ast) {
    return ConstantFP::get(*context, APFloat(ast.val));
}

Code CodeGenerator::codegen_variable_ast(const VariableExprAST &ast) {
    auto v = variables[ast.name];
    if (!v) {
        throw CodegenException("Unknown variable: " + ast.name);
    } else {
        auto c = builder->CreateLoad(v->getAllocatedType(), v, ast.name.c_str());
        return c;
    }
}

Code CodeGenerator::codegen_variable_def_ast(const LetExprAST &ast) {
    auto rhs = codegen(*ast.val);
    auto var = variables[ast.name];

    if (!var) {
        auto parentFunction = builder->GetInsertBlock()->getParent();
        llvm::IRBuilder<> TmpBuilder(&(parentFunction->getEntryBlock()),
                                     parentFunction->getEntryBlock().begin());
        llvm::AllocaInst *v = TmpBuilder.CreateAlloca(std::get<Value*>(rhs)->getType(),
                                                      nullptr,
                                                      llvm::Twine(ast.name));
        builder->CreateStore(std::get<Value*>(rhs), var);
        variables[ast.name] = v;
        return v;
    } else {
        throw CodegenException("Variable " + ast.name + " already declared");
    }
}


Code CodeGenerator::codegen_prototype_ast(const PrototypeAST &ast) {
    std::vector<Type *> Doubles(ast.args.size(), Type::getDoubleTy(*context));
    FunctionType *FT =
            FunctionType::get(Type::getDoubleTy(*context), Doubles, false);

    Function *F =
            Function::Create(FT, Function::ExternalLinkage, ast.name, *module);

    unsigned Idx = 0;
    for (auto &Arg: F->args())
        Arg.setName(ast.args[Idx++]);

    return F;
}

Code CodeGenerator::codegen_function_ast(const FunctionAST &ast) {
    auto args = ast.proto->args;
    std::vector<Type *> Doubles(args.size(),
                                Type::getDoubleTy(*context));
    FunctionType *FT =
            FunctionType::get(Type::getDoubleTy(*context), Doubles, false);

    Function *F =
            Function::Create(FT, Function::ExternalLinkage, ast.proto->name, *module);

    unsigned Idx = 0;
    for (auto &Arg: F->args())
        Arg.setName(args[Idx++]);


    BasicBlock *BB = BasicBlock::Create(*context, "entry", F);
    builder->SetInsertPoint(BB);

    // Record the function arguments in the NamedValues map.
    //(*NamedValues).clear();
    //auto tempNamedValues = *NamedValues;
    for (auto &Arg: F->args()) {
        AllocaInst *Alloca = CreateEntryBlockAlloca(F, Arg.getName().str());

        // Store the initial value into the alloca.
        builder->CreateStore(&Arg, Alloca);

        // Add arguments to variable symbol table.
        variables[std::string(Arg.getName())] = Alloca;
    }

    std::vector<Code> block;
    for (auto &expr: ast.body) {
        block.push_back(codegen(*expr));
    }

    Value *RetVal = std::get<Value*>(block.back());
    builder->CreateRet(RetVal);

    verifyFunction(*F);
    return F;
}

Code CodeGenerator::codegen_binary_ast(const BinaryExprAST &ast) {
    auto lhs_code = codegen(*ast.lhs);
    auto rhs_code = codegen(*ast.rhs);
    auto op = ast.op;


    if (op == "+")
        return builder->CreateFAdd(std::get<Value*>(lhs_code),
                                       std::get<Value*>(rhs_code), "addtmp");
    else if (op == "-")
        return builder->CreateFSub(std::get<Value*>(lhs_code),
                                       std::get<Value*>(rhs_code), "subtmp");
    else if (op == "*")
        return builder->CreateFMul(std::get<Value*>(lhs_code),
                                       std::get<Value*>(rhs_code), "multmp");
    else if (op == "<")
        return builder->CreateUIToFP(std::get<Value*>(lhs_code),
                                         Type::getDoubleTy(*context), "booltmp");
    else
        throw CodegenException("Infix operator not implemented: " + op);
}

Code CodeGenerator::codegen_callee_ast(const CallExprAST &ast) {
    auto callee = module->getFunction(ast.callee);
    if (!callee)
        throw CodegenException("Unknown function: " + ast.callee);

    auto &Args = ast.args;
    // If argument mismatch error.
    if (callee->arg_size() != Args.size())
        throw CodegenException("Incorrect # arguments passed to function: " + ast.callee);

    std::vector<Value *> ArgsV;
    for (auto &Arg: Args) {
        ArgsV.push_back(std::get<Value*>(codegen(*Arg)));
    }

    return builder->CreateCall(callee, ArgsV, "calltmp");
}

Code CodeGenerator::codegen_return_ast(const ReturnAST &ast) {
    return builder->CreateRet(std::get<Value*>(codegen(*ast.val)));
}


