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

class CodeGenVisitor {
protected:
    static constexpr size_t STACK_SIZE = 255;

    // unfortunately, these cannot be static, since they depend on context_
    // type aliases
    const llvm::IntegerType* I1_T_; // i1, or bool
    const llvm::IntegerType* I8_T_; // i8
    const llvm::IntegerType* I32_T_; // i32
    const llvm::IntegerType* I64_T_; // i64

    // int value aliases
    const llvm::ConstantInt I8_V_1_; // 1
    const llvm::ConstantInt I8_V_0_; // 0
    const llvm::ConstantInt I8_V_N1_; // -1
    //const llvm::ConstantInt I8_V_STACK_SIZE_;

public:
    CodeGenVisitor() : 
        context_{},
        module_{std::make_unique<llvm::Module>("bf_module", context)}, 
        builder_{context},
        I1_T_{llvm::Type::getInt1Ty(context_)}, 
        I8_T_{llvm::Type::getInt8Ty(context_},
        I32_T_{llvm::Type::getInt32Ty(context_},
        I64_T_{llvm::Type::getInt64Ty(context_)},
        I8_V_1_{llvm::ConstantInt::get(I8_T_, 1)},
        I8_V_0_{llvm::ConstantInt::get(I8_T_, 0)},
        I8_V_N1_{llvm::ConstantInt::get(I8_T_, -1)},
        //I8_V_STACK_SIZE_{llvm::ConstantInt::get(I8_T_, STACK_SIZE)}
    {
        // set up the main entry function and initial block
        main_func_ = llvm::Function::Create(
            llvm::FunctionType::get(I32_T_, false),
            llvm::Function::ExternalLinkage, "main", module_.get()
        );

        entry_ = llvm::BasicBlock::Create(context_, "entry", main_func_);
        builder_.SetInsertPoint(entry_);

        // define the putchar function used for print
        putchar_func_callee_ = module_->getOrInsertFunction("putchar", llvm::FunctionType::get(I8_T_, {I8_T_}, false));

        // allocate the brainfuck stack
        const llvm::ConstantInt* stack_size_i64 = llvm::ConstantInt::get(I64_T_, STACK_SIZE);
        data_ptr_ = builder.CreateAlloca(I8_T_, stack_size_i64, "stack");

        // memset stack to 0
        const llvm::Type* memset_func_t = llvm::FunctionType::get(I8_T_, {I8_T_->getPointerTo(), I8_T, I64_T_, I1_T}, false);
        FunctionCallee memset_func = module_->getOrInsertFunction("memset", );
        memset_stack_0();
    }

    // memset the stack to all zeroes
    void memset_stack_0() {
        // memset takes a i64 range
        const llvm::Value* stack_size_i64 = llvm::ConstantInt::get(I64_T_, STACK_SIZE); 
    
        // memset takes a boolean (i1) is_volatile flag
        const llvm::Value* i1_v_0 = llvm::ConstantInt::get(i1_t, false);
        const llvm::Value* is_volatile_i1 = i1_v_0; // not volatile

        builder.CreateCall(memset_func_, {data_ptr_, I8_V_0_, stack_size_i64, is_volatile_i1 });

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
        Value* inc = builder.CreateAdd(inc_load, i8_p1, "inc");
        builder.CreateStore(inc, data_ptr);
    }

    void visit(const DecStmt& stmt) {
        using namespace llvm;
        // Increment the byte at the pointer
        llvm::Value* dec_load = builder.CreateLoad(i8, data_ptr, "decLoadTmp");
        llvm::Value* dec = builder.CreateAdd(dec_load, i8_m1, "dec");
        builder.CreateStore(dec, data_ptr);
    }

    void visit(const PrintStmt& stmt) {
        llvm::Value* val = builder.CreateLoad(i8, data_ptr, "printLoadTmp");
        
        // must extend the i8 to i32
        Value* extended_val = builder.CreateZExt(val, i8, "extendedPrintLoadTmp");

        builder.CreateCall(putchar_func_callee, {extended_val});
    }

    void visit(const LoopStmt& stmt) {
    }

protected:
    llvm::LLVMContext context_;

    std::unique_ptr<llvm::Module> module_;
    llvm::BasicBlock* entry_;

    llvm::Function* main_func_;
    llvm::FunctionCallee putchar_func_callee_;

    llvm::Value* data_ptr_; // Pointer to the Brainfuck stack
    llvm::IRBuilder<> builder_;
};
