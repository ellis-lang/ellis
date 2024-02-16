//
// Created by jonathan on 12/14/23.
//

#ifndef ELLIS_COMPILER_HPP
#define ELLIS_COMPILER_HPP

#include <memory>
#include <vector>
#include <string>
#include <map>

#include "codegen.hpp"
#include "llvm-18/llvm/IR/LLVMContext.h"
#include "llvm-18/llvm/IR/IRBuilder.h"
#include "llvm-18/llvm/IR/DerivedTypes.h"
#include "llvm-18/llvm/IR/Module.h"

using namespace llvm;


class Compiler {
    bool verbose;
    std::unique_ptr<LLVMContext> TheContext;
    std::unique_ptr<IRBuilder<>> builder;
    std::unique_ptr<Module> module;
    std::map<std::string, Value *> nameValues;
    std::unique_ptr<CodeGenerator> codeGenerator;
public:
    explicit Compiler(const bool verbose) : verbose(verbose) {
        TheContext = std::make_unique<LLVMContext>();
        std::string mname = "ellis";
        module = std::make_unique<Module>(StringRef(mname), *TheContext);
        builder = std::make_unique<IRBuilder<>>(*TheContext);
        codeGenerator = std::make_unique<CodeGenerator>(CodeGenerator(*TheContext, *builder, *module, nameValues));
    }
    int compile(const std::vector<std::string>& files);
};


#endif //ELLIS_COMPILER_HPP
