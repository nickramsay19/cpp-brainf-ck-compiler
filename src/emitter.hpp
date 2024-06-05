#pragma once
#include <memory>
#include <iostream>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Intrinsics.h>
#include "ast.hpp"

#define STACK_SIZE 255

class CodeGenVisitor {
public:
    CodeGenVisitor() : module_{std::make_unique<llvm::Module>("bf_module", context)}, builder{context} {
        using namespace llvm;
        i8 = Type::getInt8Ty(context);
        i32 = Type::getInt32Ty(context);

        putchar_func_callee = module_->getOrInsertFunction("putchar", llvm::FunctionType::get(i8, {i8}, false));

        // Set up the entry function and initial block
        main_func = llvm::Function::Create(
            FunctionType::get(i32, false),
            llvm::Function::ExternalLinkage, "main", module_.get()
        );

        entry = llvm::BasicBlock::Create(context, "entry", main_func);
        builder.SetInsertPoint(entry);

        // allocate the brainfuck stack
        data_ptr = builder.CreateAlloca(i8, ConstantInt::get(i8, STACK_SIZE), "stack");

        // memset stack to 0
        Value* zero_i8 = llvm::ConstantInt::get(i8, 0);
        Value* byte_count_i64 = llvm::ConstantInt::get(llvm::Type::getInt64Ty(context), STACK_SIZE);
        Value* is_volatile_i1 = llvm::ConstantInt::get(llvm::Type::getInt1Ty(context), false);
        FunctionCallee memset_func = module_->getOrInsertFunction("memset", llvm::FunctionType::get(i8, {i8->getPointerTo(), i8, llvm::Type::getInt64Ty(context), llvm::Type::getInt1Ty(context)}, false));
        builder.CreateCall(memset_func, { data_ptr, zero_i8, byte_count_i64, is_volatile_i1 });
    }

    const llvm::Module& get_module() const {
        return *module_;
    }

    llvm::Module& get_module() {
        return *module_;
    }

    void done() {
        builder.CreateRet(builder.getInt32(0));
    }

    void visit(const LeftStmt& stmt) {
        using namespace llvm;
        std::cout << "visit:LeftStmt\n";
        data_ptr = builder.CreateGEP(i8, data_ptr, ConstantInt::get(i8, -1), "left");
    }

    void visit(const RightStmt& stmt) {
        using namespace llvm;
        data_ptr = builder.CreateGEP(i8, data_ptr, ConstantInt::get(i8, 1), "right");
    }

    void visit(const IncStmt& stmt) {
        using namespace llvm;
        // Increment the byte at the pointer
        Value* inc_load = builder.CreateLoad(i8, data_ptr, "incLoadTmp");
        Value* inc = builder.CreateAdd(inc_load, ConstantInt::get(i8, 1), "inc");
        builder.CreateStore(inc, data_ptr);
    }

    void visit(const DecStmt& stmt) {
        using namespace llvm;
        // Increment the byte at the pointer
        Value* dec_load = builder.CreateLoad(i8, data_ptr, "decLoadTmp");
        Value* dec = builder.CreateAdd(dec_load, ConstantInt::get(i8, -1), "dec");
        builder.CreateStore(dec, data_ptr);
    }

    void visit(const PrintStmt& stmt) {
        using namespace llvm;
        Value* val = builder.CreateLoad(i8, data_ptr, "printLoadTmp");
        
        // must extend the i8 to i32
        Value* extended_val = builder.CreateZExt(val, i8, "extendedPrintLoadTmp");

        builder.CreateCall(putchar_func_callee, {extended_val});
    }

    void visit(const LoopStmt& stmt) {
        using namespace llvm;
    }

protected:
    llvm::LLVMContext context;

    std::unique_ptr<llvm::Module> module_;
    llvm::BasicBlock* entry;

    llvm::Function* main_func;
    llvm::FunctionCallee putchar_func_callee;

    llvm::Value* data_ptr; // Pointer to the Brainfuck stack
    llvm::IRBuilder<> builder;

    llvm::IntegerType* i8;
    llvm::IntegerType* i32;
};
