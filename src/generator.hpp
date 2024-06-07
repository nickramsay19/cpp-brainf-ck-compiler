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

class Generator {
protected:
    static constexpr size_t STACK_SIZE = 255;

    // unfortunately, these cannot be static, since they depend on context_
    // furthermore they cannot be constant, since llvm doesn't account for
    // ... constant variants, even though they should never be changed
    // still give them fully uppercase names as if they were static constants

    // type aliases
    llvm::IntegerType* I1_T_; // i1, or bool
    llvm::IntegerType* I8_T_; // i8
    llvm::IntegerType* I32_T_; // i32
    llvm::IntegerType* I64_T_; // i64

    // common int value aliases
    llvm::ConstantInt* I8_V_1_; // 1
    llvm::ConstantInt* I8_V_0_; // 0
    llvm::ConstantInt* I8_V_N1_; // -1

public:
    Generator();

    llvm::Module& get_module();

    void visit(const Program* prog);

    void visit(const LeftStmt& stmt);

    void visit(const RightStmt& stmt);

    void visit(const IncStmt& stmt);

    void visit(const DecStmt& stmt);

    void visit(const PrintStmt& stmt);

    void visit(const LoopStmt& stmt);

protected:
    llvm::LLVMContext context_;

    std::unique_ptr<llvm::Module> module_; // the module to construct

    llvm::Function* main_func_;
    llvm::FunctionCallee putchar_func_callee_;
    llvm::FunctionCallee memset_func_callee_;

    llvm::BasicBlock* entry_; // entry point aka main

    llvm::Value* head_; // pointer to the stack head
    llvm::IRBuilder<> builder_;
};
