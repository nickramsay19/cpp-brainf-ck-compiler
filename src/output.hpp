#pragma once
#include <iostream>
#include <string>
#include <optional>
#include <sstream>
#include <vector>

#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>

#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/FileSystem.h>

#include <llvm/IRReader/IRReader.h>
#include <llvm/Bitcode/BitcodeWriter.h>
#include <llvm/IR/DebugInfo.h>
#include <llvm/Analysis/Passes.h>

#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>

#include <llvm-c/Analysis.h>
#include <llvm/IR/Verifier.h>
#include <llvm/InitializePasses.h>
#include <llvm/PassRegistry.h>

#include <llvm/MC/TargetRegistry.h>
#include <llvm/TargetParser/Host.h>

#include <llvm/Bitcode/BitcodeWriter.h>
#include <llvm/Object/ObjectFile.h>
#include <llvm/Support/CodeGen.h>

#include <llvm/Passes/PassBuilder.h>
#include <llvm/IR/PassManager.h>
#include <llvm/Passes/PassPlugin.h>

#include <llvm/ADT/StringRef.h>

#include <lld/Common/Driver.h>

class LLVMModuleEmitter {
public:
    LLVMModuleEmitter(
        llvm::Module& module_, 
        std::string target_triple
    ) : 
        module_{module_}, 
        target_triple_{target_triple}, 
        target_{llvm::TargetRegistry::lookupTarget(target_triple, err_)}, 
        target_opt_{}
    {

        // target machine    
        std::string err;
        const llvm::Target* target = llvm::TargetRegistry::lookupTarget(target_triple, err);

        if (!target) {
            std::cerr << err;
            return;
        }

        std::optional<llvm::Reloc::Model> reloc {llvm::Reloc::PIC_};

        target_machine_ = target->createTargetMachine(
            target_triple_, 
            "generic",
            "",
            target_opt_, 
            reloc
        );

        module_.setDataLayout(target_machine_->createDataLayout());
        module_.setTargetTriple(target_triple_);
    }

    int emit(std::ostream* output_stream_ptr, llvm::CodeGenFileType file_type) {

        OStreamToLLVMRawPWriteStreamAdaptor llvm_output_stream {output_stream_ptr};

        llvm::legacy::PassManager pass_mgr;
        if (target_machine_->addPassesToEmitFile(pass_mgr, llvm_output_stream, nullptr, file_type)) {
            std::cerr << "TargetMachine can't emit a file of this type";
            return 1;
        }

        pass_mgr.run(module_);
        llvm_output_stream.flush();

        return 0;
    }

protected:
    llvm::Module& module_;    

    std::string err_;
    const llvm::Target* target_;
    llvm::TargetOptions target_opt_;

    // target machine
    std::string target_triple_;
    llvm::TargetMachine* target_machine_;

};
