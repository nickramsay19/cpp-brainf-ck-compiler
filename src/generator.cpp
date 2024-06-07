#include "generator.hpp"
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

Generator::Generator() : 
        context_{},
        module_{std::make_unique<llvm::Module>("bf_module", context_)}, 
        builder_{context_}
{
    I1_T_ = llvm::Type::getInt1Ty(context_);
    I8_T_ = llvm::Type::getInt8Ty(context_);
    I32_T_ = llvm::Type::getInt32Ty(context_);
    I64_T_ = llvm::Type::getInt64Ty(context_);

    I8_V_1_ = llvm::ConstantInt::get(I8_T_, 1);
    I8_V_0_ = llvm::ConstantInt::get(I8_T_, 0);
    I8_V_N1_ = llvm::ConstantInt::get(I8_T_, -1);

    // set up the main entry function and initial block
    main_func_ = llvm::Function::Create(
        llvm::FunctionType::get(I32_T_, false),
        llvm::Function::ExternalLinkage, "main", module_.get()
    );

    // define main as the entry point
    entry_ = llvm::BasicBlock::Create(context_, "entry", main_func_);

    // define the putchar function used for print
    putchar_func_callee_ = module_->getOrInsertFunction("putchar", llvm::FunctionType::get(I8_T_, {I8_T_}, false));

    // define the memset func, needed for clearing stack
    memset_func_callee_ = module_->getOrInsertFunction("memset", llvm::FunctionType::get(I8_T_, {I8_T_->getPointerTo(), I8_T_, I64_T_, I1_T_}, false));
}


llvm::Module& Generator::get_module() {
    return *module_;
}

void Generator::visit(const Program* prog) {

    builder_.SetInsertPoint(entry_);

    llvm::Value* stack_size_i64 = llvm::ConstantInt::get(I64_T_, STACK_SIZE);

    // set the head pointer
    head_ = builder_.CreateAlloca(I8_T_, stack_size_i64, "stack");

    // memset the stack to all zeroes
    llvm::Value* is_volatile_i1 = llvm::ConstantInt::get(I1_T_, false); // 0 for non-volatile
    builder_.CreateCall(memset_func_callee_, {head_, I8_V_0_, stack_size_i64, is_volatile_i1 });
    
    // generate program stmt_list code
    prog->stmt_list->accept(*this);

    //builder_.CreateRet(builder_.getInt32(0));
    builder_.CreateRet(llvm::ConstantInt::get(I32_T_, 0));
}

void Generator::visit(const LeftStmt& stmt) {
    head_ = builder_.CreateGEP(I8_T_, head_, I8_V_N1_, "left");
}

void Generator::visit(const RightStmt& stmt) {
    head_ = builder_.CreateGEP(I8_T_, head_, I8_V_1_, "right");
}

void Generator::visit(const IncStmt& stmt) {
    llvm::Value* inc_load = builder_.CreateLoad(I8_T_, head_, "incLoadTmp");
    llvm::Value* inc = builder_.CreateAdd(inc_load, I8_V_1_, "inc");
    builder_.CreateStore(inc, head_);
}

void Generator::visit(const DecStmt& stmt) {
    llvm::Value* dec_load = builder_.CreateLoad(I8_T_, head_, "decLoadTmp");
    llvm::Value* dec = builder_.CreateAdd(dec_load, I8_V_N1_, "dec");
    builder_.CreateStore(dec, head_);
}

void Generator::visit(const PrintStmt& stmt) {
    llvm::Value* val = builder_.CreateLoad(I8_T_, head_, "printLoadTmp");
    
    // must extend the i8 to i32
    llvm::Value* extended_val = builder_.CreateZExt(val, I8_T_, "extendedPrintLoadTmp");

    builder_.CreateCall(putchar_func_callee_, {extended_val});
}

void Generator::visit(const LoopStmt& stmt) {
    llvm::BasicBlock* loop = llvm::BasicBlock::Create(context_, "loop", main_func_);
    llvm::BasicBlock* done = llvm::BasicBlock::Create(context_, "done", main_func_);

    // start loop
    builder_.CreateBr(loop);
    builder_.SetInsertPoint(loop);

    // visit stmt list which is loop body code
    stmt.stmt_list->accept(*this);

    // end of loop, check cond
    llvm::Value* head_load = builder_.CreateLoad(I8_T_, head_, "headLoadLoopCondCheckTmp");
    llvm::Value* cond = builder_.CreateICmpEQ(head_load, I8_V_0_, "loopCond");

    //builder_.CreateCondBr(cond, loop, done);
    builder_.CreateCondBr(cond, done, loop);

    builder_.SetInsertPoint(done);
}












