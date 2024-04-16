//
// Created by jonathan on 4/16/24.
//

#ifndef ELLIS_ELLIS_JIT_HPP
#define ELLIS_ELLIS_JIT_HPP

#include "llvm/ADT/StringRef.h"
#include "llvm/ExecutionEngine/JITSymbol.h"
#include "llvm/ExecutionEngine/Orc/CompileUtils.h"
#include "llvm/ExecutionEngine/Orc/Core.h"
#include "llvm/ExecutionEngine/Orc/ExecutionUtils.h"
#include "llvm/ExecutionEngine/Orc/ExecutorProcessControl.h"
#include "llvm/ExecutionEngine/Orc/IRCompileLayer.h"
#include "llvm/ExecutionEngine/Orc/JITTargetMachineBuilder.h"
#include "llvm/ExecutionEngine/Orc/RTDyldObjectLinkingLayer.h"
#include "llvm/ExecutionEngine/SectionMemoryManager.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/LLVMContext.h"
#include <memory>

using namespace llvm;

class EllisJIT {
private:
    std::unique_ptr<llvm::orc::ExecutionSession> ES;

    DataLayout DL;
    llvm::orc::MangleAndInterner Mangle;

    llvm::orc::RTDyldObjectLinkingLayer ObjectLayer;
    llvm::orc::IRCompileLayer CompileLayer;

    llvm::orc::JITDylib &MainJD;

public:
    EllisJIT(std::unique_ptr<llvm::orc::ExecutionSession> ES,
             llvm::orc::JITTargetMachineBuilder JTMB, DataLayout DL)
            : ES(std::move(ES)), DL(std::move(DL)), Mangle(*this->ES, this->DL),
              ObjectLayer(*this->ES,
                          []() { return std::make_unique<SectionMemoryManager>(); }),
              CompileLayer(*this->ES, ObjectLayer,
                           std::make_unique<llvm::orc::ConcurrentIRCompiler>(std::move(JTMB))),
              MainJD(this->ES->createBareJITDylib("<main>")) {
        MainJD.addGenerator(
                cantFail(llvm::orc::DynamicLibrarySearchGenerator::GetForCurrentProcess(
                        DL.getGlobalPrefix())));
        if (JTMB.getTargetTriple().isOSBinFormatCOFF()) {
            ObjectLayer.setOverrideObjectFlagsWithResponsibilityFlags(true);
            ObjectLayer.setAutoClaimResponsibilityForObjectSymbols(true);
        }
    }

    ~EllisJIT() {
        if (auto Err = ES->endSession())
            ES->reportError(std::move(Err));
    }

    static llvm::Expected<std::unique_ptr<EllisJIT>> Create() {
        auto EPC = llvm::orc::SelfExecutorProcessControl::Create();
        if (!EPC)
            return EPC.takeError();

        auto ES = std::make_unique<llvm::orc::ExecutionSession>(std::move(*EPC));

        llvm::orc::JITTargetMachineBuilder JTMB(
                ES->getExecutorProcessControl().getTargetTriple());

        auto DL = JTMB.getDefaultDataLayoutForTarget();
        if (!DL)
            return DL.takeError();

        return std::make_unique<EllisJIT>(std::move(ES), std::move(JTMB),
                                          std::move(*DL));
    }

    const DataLayout &getDataLayout() const { return DL; }

    llvm::orc::JITDylib &getMainJITDylib() { return MainJD; }

    Error addModule(llvm::orc::ThreadSafeModule TSM, llvm::orc::ResourceTrackerSP RT = nullptr) {
        if (!RT)
            RT = MainJD.getDefaultResourceTracker();
        return CompileLayer.add(RT, std::move(TSM));
    }

    Expected<JITEvaluatedSymbol> lookup(StringRef Name) {
        return ES->lookup({&MainJD}, Mangle(Name.str()));
    }
};


#endif //ELLIS_ELLIS_JIT_HPP
