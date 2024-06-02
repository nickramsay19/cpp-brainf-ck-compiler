#include "ast.hpp"
#include <memory>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include "emitter.hpp"

void Program::accept(CodeGenVisitor& visitor) const {
    stmt_list->accept(visitor);
    visitor.done(); // mark main method finished
}

void FullStmtList::accept(CodeGenVisitor& visitor) const {
    stmt->accept(visitor);
    next->accept(visitor); // recurse
}

void EmptyStmtList::accept(CodeGenVisitor& visitor) const {}

void LeftStmt::accept(CodeGenVisitor& visitor) const {
    visitor.visit(*this);
}

void RightStmt::accept(CodeGenVisitor& visitor) const {
    visitor.visit(*this);
}

void IncStmt::accept(CodeGenVisitor& visitor) const {
    visitor.visit(*this);
}

void PrintStmt::accept(CodeGenVisitor& visitor) const {
    visitor.visit(*this);
}
