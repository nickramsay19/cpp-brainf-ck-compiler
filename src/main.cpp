#include <iostream>
#include <string>
#include <fstream>
#include <memory>
#include <utility>

#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>

#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/FileSystem.h>

#include <llvm/IRReader/IRReader.h>
#include <llvm/Bitcode/BitcodeWriter.h>
#include <llvm/IR/DebugInfo.h>
#include <llvm/Analysis/Passes.h>

//#include <llvm/ADT/Optional.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/FileSystem.h>
//#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>

#include <llvm-c/Analysis.h>
#include <llvm/IR/Verifier.h>
#include <llvm/InitializePasses.h>
#include <llvm/PassRegistry.h>

#include "parser.hpp"
#include "ast.hpp"
#include "emitter.hpp"

int write_llvm_ir(const llvm::Module& mod, std::string file_name) {
    std::error_code EC;
    //raw_fd_ostream OS("output.ll", EC);  // Simply remove the flag
    raw_fd_ostream OS(file_name, EC);  // Simply remove the flag

    if (EC) {
        std::cerr << "Error opening file for output: " << EC.message() << '\n';
        return 1;
    }

    mod.print(OS, nullptr);
    OS.flush(); // Ensure all content is written to the file

    return 0;
}

int main(int argc, char* argv[]) {

    if (argc < 2) {
        std::cerr << "No input file provided\n";
        return 1;
    }

    std::fstream file(argv[1], std::ios::in);
    if (!file.is_open()) {
        std::cerr << "Failed to open file \"" << argv[1] << "\"\n";
        return 1;
    }

    Parser parser(file);    
    const Program program = parser.parse();


    /*const Program program(
        std::make_unique<FullStmtList>(
            std::move(std::make_unique<IncStmt>()), std::move(std::make_unique<FullStmtList>(
            std::move(std::make_unique<IncStmt>()), std::move(std::make_unique<FullStmtList>(
            std::move(std::make_unique<IncStmt>()), std::move(std::make_unique<FullStmtList>(
            std::move(std::make_unique<IncStmt>()), std::move(std::make_unique<FullStmtList>(
            std::move(std::make_unique<IncStmt>()), std::move(std::make_unique<FullStmtList>(
            std::move(std::make_unique<IncStmt>()), std::move(std::make_unique<FullStmtList>(
            std::move(std::make_unique<IncStmt>()), std::move(std::make_unique<FullStmtList>(
            std::move(std::make_unique<IncStmt>()), std::move(std::make_unique<FullStmtList>(
            std::move(std::make_unique<IncStmt>()), std::move(std::make_unique<FullStmtList>(
            std::move(std::make_unique<IncStmt>()), std::move(std::make_unique<FullStmtList>(
            std::move(std::make_unique<IncStmt>()), std::move(std::make_unique<FullStmtList>(
            std::move(std::make_unique<IncStmt>()), std::move(std::make_unique<FullStmtList>(
            std::move(std::make_unique<IncStmt>()), std::move(std::make_unique<FullStmtList>(
            std::move(std::make_unique<IncStmt>()), std::move(std::make_unique<FullStmtList>(
            std::move(std::make_unique<IncStmt>()), std::move(std::make_unique<FullStmtList>(
            std::move(std::make_unique<IncStmt>()), std::move(std::make_unique<FullStmtList>(
            std::move(std::make_unique<IncStmt>()), std::move(std::make_unique<FullStmtList>(
            std::move(std::make_unique<IncStmt>()), std::move(std::make_unique<FullStmtList>(
            std::move(std::make_unique<IncStmt>()), std::move(std::make_unique<FullStmtList>(
            std::move(std::make_unique<IncStmt>()), std::move(std::make_unique<FullStmtList>(
            std::move(std::make_unique<IncStmt>()), std::move(std::make_unique<FullStmtList>(
            std::move(std::make_unique<IncStmt>()), std::move(std::make_unique<FullStmtList>(
            std::move(std::make_unique<IncStmt>()), std::move(std::make_unique<FullStmtList>(
            std::move(std::make_unique<IncStmt>()), std::move(std::make_unique<FullStmtList>(
            std::move(std::make_unique<IncStmt>()), std::move(std::make_unique<FullStmtList>(
            std::move(std::make_unique<IncStmt>()), std::move(std::make_unique<FullStmtList>(
            std::move(std::make_unique<IncStmt>()), std::move(std::make_unique<FullStmtList>(
            std::move(std::make_unique<IncStmt>()), std::move(std::make_unique<FullStmtList>(
            std::move(std::make_unique<IncStmt>()), std::move(std::make_unique<FullStmtList>(
            std::move(std::make_unique<IncStmt>()), std::move(std::make_unique<FullStmtList>(
            std::move(std::make_unique<IncStmt>()), std::move(std::make_unique<FullStmtList>(
            std::move(std::make_unique<IncStmt>()), std::move(std::make_unique<FullStmtList>(
            std::move(std::make_unique<IncStmt>()), std::move(std::make_unique<FullStmtList>(
            std::move(std::make_unique<IncStmt>()), std::move(std::make_unique<FullStmtList>(
            std::move(std::make_unique<PrintStmt>()), std::move(std::make_unique<EmptyStmtList>())))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))
        )
    );*/

    CodeGenVisitor visitor;
    program.accept(visitor);

    const llvm::Module& mod = visitor.get_module();
    llvm::verifyModule(mod, &llvm::errs());
    write_llvm_ir(mod, std::string {"output.ll"});

    return 0;
}
