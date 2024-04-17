//
// Created by jonathan on 12/14/23.
//

#ifndef ELLIS_COMPILER_HPP
#define ELLIS_COMPILER_HPP

#include <memory>
#include <utility>
#include <vector>
#include <string>
#include <map>

#include "codegen.hpp"
#include "lex.hpp"
#include "parser.hpp"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Module.h"
#include "llvm/Analysis/LoopAnalysisManager.h"
#include "llvm/Analysis/CGSCCPassManager.h"
#include "llvm/Passes/StandardInstrumentations.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar/Reassociate.h"
#include "llvm/Transforms/Scalar/GVN.h"
#include "llvm/Transforms/Scalar/SimplifyCFG.h"
#include "llvm/ExecutionEngine/Orc/ThreadSafeModule.h"
#include "llvm/Support/TargetSelect.h"
#include "ellis_jit.hpp"

using namespace llvm;


class Compiler {
    bool verbose;
    std::unique_ptr<LLVMContext> TheContext;
    std::unique_ptr<IRBuilder<>> builder;
    std::unique_ptr<Module> module;
    std::map<std::string, AllocaInst *> namedValues;
    std::unique_ptr<CodeGenerator> codeGenerator;
    std::unique_ptr<EllisJIT> TheJIT;

    std::unique_ptr<FunctionPassManager> TheFPM;
    std::unique_ptr<LoopAnalysisManager> TheLAM;
    std::unique_ptr<FunctionAnalysisManager> TheFAM;
    std::unique_ptr<CGSCCAnalysisManager> TheCGAM;
    std::unique_ptr<ModuleAnalysisManager> TheMAM;
    std::unique_ptr<PassInstrumentationCallbacks> ThePIC;
    std::unique_ptr<StandardInstrumentations> TheSI;
    std::map<std::string, std::unique_ptr<PrototypeAST>> FunctionProtos;
    ExitOnError ExitOnErr;
public:
    void ReinitializeModuleAndManagers() {
        // Open a new context and module.
        TheContext = std::make_unique<LLVMContext>();
        module = std::make_unique<Module>("EllisJIT", *TheContext);
        module->setDataLayout(TheJIT->getDataLayout());
        // Create a new builder for the module.
        builder = std::make_unique<IRBuilder<>>(*TheContext);
        // Create new pass and analysis managers.
        TheFPM = std::make_unique<FunctionPassManager>();
        TheLAM = std::make_unique<LoopAnalysisManager>();
        TheFAM = std::make_unique<FunctionAnalysisManager>();
        TheCGAM = std::make_unique<CGSCCAnalysisManager>();
        TheMAM = std::make_unique<ModuleAnalysisManager>();
        ThePIC = std::make_unique<PassInstrumentationCallbacks>();
        TheSI = std::make_unique<StandardInstrumentations>(false);
        TheSI->registerCallbacks(*ThePIC, reinterpret_cast<FunctionAnalysisManager *>(TheMAM.get()));
        // Add transform passes.
        // Do simple "peephole" optimizations and bit-twiddling optzns.
        TheFPM->addPass(InstCombinePass());
        // Reassociate expressions.
        TheFPM->addPass(ReassociatePass());
        // Eliminate Common SubExpressions.
        TheFPM->addPass(GVNPass());
        // Simplify the control flow graph (deleting unreachable blocks, etc).
        TheFPM->addPass(SimplifyCFGPass());

        // Register analysis passes used in these transform passes.
        PassBuilder PB;
        PB.registerModuleAnalyses(*TheMAM);
        PB.registerFunctionAnalyses(*TheFAM);
        PB.crossRegisterProxies(*TheLAM, *TheFAM, *TheCGAM, *TheMAM);
        codeGenerator = std::make_unique<CodeGenerator>(CodeGenerator(*TheContext, *builder, *module, namedValues));
    }

    void InitializeModuleAndManagers() {
        // Open a new context and module.
        TheContext = std::make_unique<LLVMContext>();
        module = std::make_unique<Module>("EllisJIT", *TheContext);
        module->setDataLayout(TheJIT->getDataLayout());
        // Create a new builder for the module.
        builder = std::make_unique<IRBuilder<>>(*TheContext);
        // Create new pass and analysis managers.
        TheFPM = std::make_unique<FunctionPassManager>();
        TheLAM = std::make_unique<LoopAnalysisManager>();
        TheFAM = std::make_unique<FunctionAnalysisManager>();
        TheCGAM = std::make_unique<CGSCCAnalysisManager>();
        TheMAM = std::make_unique<ModuleAnalysisManager>();
        ThePIC = std::make_unique<PassInstrumentationCallbacks>();
        TheSI = std::make_unique<StandardInstrumentations>(false);
        TheSI->registerCallbacks(*ThePIC, reinterpret_cast<FunctionAnalysisManager *>(TheMAM.get()));
        // Add transform passes.
        // Do simple "peephole" optimizations and bit-twiddling optzns.
        TheFPM->addPass(InstCombinePass());
        // Reassociate expressions.
        TheFPM->addPass(ReassociatePass());
        // Eliminate Common SubExpressions.
        TheFPM->addPass(GVNPass());
        // Simplify the control flow graph (deleting unreachable blocks, etc).
        TheFPM->addPass(SimplifyCFGPass());

        // Register analysis passes used in these transform passes.
        PassBuilder PB;
        PB.registerModuleAnalyses(*TheMAM);
        PB.registerFunctionAnalyses(*TheFAM);
        PB.crossRegisterProxies(*TheLAM, *TheFAM, *TheCGAM, *TheMAM);
        codeGenerator = std::make_unique<CodeGenerator>(CodeGenerator(*TheContext, *builder, *module, namedValues));
    }

    explicit Compiler(const bool verbose) : verbose(verbose) {
        InitializeNativeTarget();
        InitializeNativeTargetAsmPrinter();
        InitializeNativeTargetAsmParser();
        TheJIT = ExitOnErr(EllisJIT::Create());
        InitializeModuleAndManagers();
    }

    int compile(const std::vector<std::string>& files);

    int jit(std::string& source) {
        auto tokens = lex(std::move(source), false);
        auto asts = parse(tokens);
        auto Proto = std::make_unique<PrototypeAST>("__anon_expr",
                                                    std::vector<std::string>());
        auto anon_fn = FunctionAST(std::move(Proto), std::move(asts));
        anon_fn.Accept(*codeGenerator);
        anon_fn.getCode().v->print(errs());
        printf("\n");

        namedValues = codeGenerator->NamedValues;

        auto RT = TheJIT->getMainJITDylib().createResourceTracker();

        auto TSM = llvm::orc::ThreadSafeModule(std::move(module), std::move(TheContext));
        ExitOnErr(TheJIT->addModule(std::move(TSM), RT));
        ReinitializeModuleAndManagers();

        auto ExprSymbol = ExitOnErr(TheJIT->lookup("__anon_expr"));
        auto *FP = jitTargetAddressToPointer<double (*)()>(ExprSymbol.getAddress());

        fprintf(stderr, "Evaluated to %f\n", FP());

        // Delete the anonymous expression module from the JIT.
        ExitOnErr(RT->remove());

        return 0;
    }
};


#endif //ELLIS_COMPILER_HPP
