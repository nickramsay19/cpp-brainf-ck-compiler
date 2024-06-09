#include <iostream>
#include <string>
#include <fstream>
#include <optional>
#include <memory>
#include <utility>
#include <stdexcept>
#include <variant>

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

//#include <llvm-c/Analysis.h>
#include <llvm/IR/Verifier.h>

#include <llvm/Support/TargetSelect.h> // for initialization functions
#include <llvm/Bitcode/BitcodeWriter.h>
#include <llvm/Support/CodeGen.h>

#include "argparse/argparse.hpp"

#include "parser.hpp"
#include "ast.hpp"
#include "generator.hpp"
#include "ostream_to_llvm_raw_pwrite_stream_adaptor.hpp"
#include "output.hpp"

template <typename T> requires std::is_same_v<T, std::istream> || std::is_same_v<T, std::ostream>
int open_fstream_overwrite_ptr(const std::string& file_name, std::unique_ptr<std::fstream>& managed, T** ptr_to_unmanaged, const std::ios_base::openmode& mode) {
    managed.reset(new std::fstream (file_name, mode));
    if (!managed->is_open()) {
        return 1;
    }

    *ptr_to_unmanaged = managed.get();

    return 0;
}

int main(int argc, char* argv[]) {

    // parse cli options
    argparse::ArgumentParser arg_parser("bfc");
    arg_parser.add_argument("-o", "--output")
        .help("Output executable file name")
        .default_value(std::string("a.out"));

    arg_parser.add_group("Stage Selection Options");
    auto& group = arg_parser.add_mutually_exclusive_group();
    group.add_argument("-S", "--asm")
        .help("LLVM generation and optimization stages and target-specific code generation, producing an assembly file")
        .flag();
    group.add_argument("-c", "--compile")
        .help("The above, plus the assembler, generating a target \".o\" object file.")
        .flag();
    group.add_argument("-e", "--exe")
        .help("the above, plus linking to produce an executable")
        .flag()
        .default_value(true);

    arg_parser.add_group("Code Generation Options");
    arg_parser.add_hidden_alias_for(arg_parser.add_argument("-l", "--llvm-ir", "--emit-llvm")
        .help("Emit LLVM IR")
        .flag(), "-emit-llvm");

    arg_parser.add_argument("input")
        .help("Input file name")
        .default_value(std::string("-"));

    try {
        arg_parser.parse_args(argc, argv);
    } catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << arg_parser;
        std::exit(1);
    }

    // input file
    const std::string input_file_name = arg_parser.get<std::string>("input");

    // by default, read from stdin
    // keep an unmanaged pointer to it, since it cannot be freed
    std::istream* input_ptr = &std::cin;

    // allocate a fstream, incase we end up using one
    // place it in a unque_ptr in the current (main) scope, 
    // unlike std::cin, it can and must be freed
    std::unique_ptr<std::fstream> managed_input_ptr;
    if (input_file_name != "-" && open_fstream_overwrite_ptr(input_file_name, managed_input_ptr, &input_ptr, std::ios::in)) {
        std::cerr << "Failed to open file \"" << input_file_name << "\"\n";
        return 1;
    }

    // parse input code
    Parser parser (*input_ptr); 
    std::unique_ptr<Program> program = parser.parse();
    //std::cerr << static_cast<std::string>(*program) << '\n';

    // generate llvm module
    Generator generator;
    program->accept(generator);
    llvm::Module& module_ = generator.get_module();

    // verify module
    if (llvm::verifyModule(module_, &llvm::errs())) {
        std::cerr << "Generated module has errors";
        return 1;
    }

    // open output file
    std::string output_file_name = arg_parser.get<std::string>("output");
    std::ostream* output_ptr = &std::cout;
    std::unique_ptr<std::fstream> managed_output_ptr;
    if (output_file_name != "-" && open_fstream_overwrite_ptr(output_file_name, managed_output_ptr, &output_ptr, std::ios::out)) {
        std::cerr << "Failed to open file \"" << output_file_name << " for writing\"\n";
        return 1;
    }

    // generate final output
    if (arg_parser.get<bool>("--emit-llvm")) {
        OStreamToLLVMRawPWriteStreamAdaptor llvm_output {output_ptr};
        if (arg_parser.get<bool>("--asm")) {
            module_.print(llvm_output, nullptr); // write LLVM IR
        } else {
            llvm::WriteBitcodeToFile(module_, llvm_output); // llvm bitcode format obj files
        }     

        llvm_output.flush();
    } else {

        llvm::InitializeAllTargetInfos();
        llvm::InitializeAllTargets();
        llvm::InitializeAllTargetMCs();
        llvm::InitializeAllAsmPrinters();

        LLVMModuleEmitter emitter(module_, llvm::sys::getDefaultTargetTriple());

        if (arg_parser.get<bool>("--asm")) {
            emitter.emit(&std::cout, llvm::CodeGenFileType::CGFT_AssemblyFile);
        } else if (arg_parser.get<bool>("--compile")) {
            emitter.emit(&std::cout, llvm::CodeGenFileType::CGFT_ObjectFile);
        } else {
            std::cout << "TODO\n";
        }
    }

    return 0;
}
