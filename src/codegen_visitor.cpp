//
// Created by jon on 4/13/24.
//

#include "codegen_visitor.h"

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


CodegenVisitor::CodegenVisitor() {
    context = std::make_unique<llvm::LLVMContext>();
    builder = std::make_unique<llvm::IRBuilder<>>(*context);
    module = std::make_unique<llvm::Module>("Module", *context);
}

void CodegenVisitor::dumpLLVMIR() { module->print(llvm::outs(), nullptr); }

llvm::Value *CodegenVisitor::codegen(const NumberExprAST &expr) {
    return nullptr;
}

llvm::Value *CodegenVisitor::codegen(const CharExprAST &expr) {
    return nullptr;
}

llvm::Value *CodegenVisitor::codegen(const StringExprAST &expr) {
    return nullptr;
}

llvm::Value *CodegenVisitor::codegen(const VariableExprAST &expr) {
    llvm::Value *val = varEnv[expr];
    if (val == nullptr) {
        throw new IRCodegenException(std::string("Var not found: " + var.varName));
    }
    return val;
}

llvm::Value *CodegenVisitor::codegen(const VariableDefAST &expr) {
    return nullptr;
}

llvm::Value *CodegenVisitor::codegen(const BinaryExprAST &expr) {
    return nullptr;
}

llvm::Value *CodegenVisitor::codegen(const CallExprAST &expr) {
    return nullptr;
}

llvm::Value *CodegenVisitor::codegen(const Function &expr) {
    return nullptr;
}

llvm::Value *CodegenVisitor::codegen(const IfAST &expr) {
    return nullptr;
}



