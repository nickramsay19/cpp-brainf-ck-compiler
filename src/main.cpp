#include <iostream>
#include <string>
#include <fstream>
#include <optional>
#include <memory>
#include <utility>
#include <stdexcept>

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

//#include <llvm/Support/TargetRegistry.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/TargetParser/Host.h>

#include "argparse/argparse.hpp"

#define DEBUG_PRINT 0

#include "parser.hpp"
#include "ast.hpp"
#include "emitter.hpp"

void emitObjectFile(llvm::Module& mod) {
    // Initialize the target registry etc.
    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmParsers();
    llvm::InitializeAllAsmPrinters();

    auto TargetTriple = llvm::sys::getDefaultTargetTriple();
    mod.setTargetTriple(TargetTriple);

    std::string Error;
    auto Target = llvm::TargetRegistry::lookupTarget(TargetTriple, Error);

    // Print an error and exit if we couldn't find the requested target.
    // This generally occurs if we've forgotten to initialise the
    // TargetRegistry or we have a bogus target triple.
    if (!Target) {
        llvm::errs() << Error;
        return;
    }

    auto CPU = "generic";
    auto Features = "";

    llvm::TargetOptions opt;
    //auto RM = llvm::Optional<llvm::Reloc::Model>();
    std::optional<llvm::Reloc::Model> RM;

    auto TheTargetMachine = Target->createTargetMachine(TargetTriple, CPU, Features, opt, RM);

    mod.setDataLayout(TheTargetMachine->createDataLayout());

    std::error_code EC;
    llvm::raw_fd_ostream dest("output.o", EC, llvm::sys::fs::OF_None);

    if (EC) {
        llvm::errs() << "Could not open file: " << EC.message();
        return;
    }

    llvm::legacy::PassManager pass;
    auto FileType = llvm::CGFT_ObjectFile;

    if (TheTargetMachine->addPassesToEmitFile(pass, dest, nullptr, FileType)) {
        llvm::errs() << "TheTargetMachine can't emit a file of this type";
        return;
    }

    pass.run(mod);
    dest.flush();
}

int writeLLVMIR(const llvm::Module& mod, std::string file_name) {
    std::error_code EC;
    //raw_fd_ostream OS("output.ll", EC);  // Simply remove the flag
    llvm::raw_fd_ostream OS(file_name, EC);  // Simply remove the flag

    if (EC) {
        std::cerr << "Error opening file for output: " << EC.message() << '\n';
        return 1;
    }

    mod.print(OS, nullptr);
    OS.flush(); // Ensure all content is written to the file

    return 0;
}

int main(int argc, char* argv[]) {

    // parse cli options
    argparse::ArgumentParser arg_parser("bfc");
    arg_parser.add_argument("-o", "--output")
        .help("output executable file name")
        .default_value(std::string("a.out"));

    arg_parser.add_group("Stage Selection Options");
    auto& group = arg_parser.add_mutually_exclusive_group();
    group.add_argument("-S")
        .help("Run the previous stages as well as LLVM generation and optimization stages and target-specific code generation, producing an assembly file.")
        .flag();
    group.add_argument("-c")
        .help("Run all of the above, plus the assembler, generating a target \".o\" object file.")
        .flag();

    arg_parser.add_group("Code Generation Options");
    arg_parser.add_hidden_alias_for(arg_parser.add_argument("-l", "--emit-llvm")
        .help("emit LLVM IR")
        .flag(), "-emit-llvm");

    arg_parser.add_argument("input")
        .help("input brainfuck file name")
        .default_value(std::string("-"));

    try {
        arg_parser.parse_args(argc, argv);
    }
    catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << arg_parser;
        std::exit(1);
    }

    // by default, read from stdin
    std::istream* input_ptr = &std::cin;

    // allocate a fstream, incase we end up using one
    std::string file_name = arg_parser.get<std::string>("input");

    std::unique_ptr<std::fstream> file_input_ptr;

    // check if using a file
    if (file_name != "-") {
        file_input_ptr.reset(new std::fstream (file_name, std::ios::in));
        if (!file_input_ptr->is_open()) {
            std::cerr << "Failed to open file \"" << file_name << "\"\n";
            return 1;
        }

        input_ptr = file_input_ptr.get();
    }

    // parse input code
    Parser parser (*input_ptr); 

    std::unique_ptr<Program> program = parser.parse();
    std::cout << static_cast<std::string>(*program) << '\n';

    // generate llvm
    CodeGenVisitor visitor;
    program->accept(visitor);
    const llvm::Module& mod = visitor.get_module();

    // verify module
    if (llvm::verifyModule(mod, &llvm::errs())) {
        throw std::runtime_error("Generated module has errors");
    }

    // generate final output
    if (arg_parser.get<bool>("--emit-llvm")) {
        // write LLVM IR
        writeLLVMIR(mod, std::string {"output.ll"});
    }

    return 0;
}
