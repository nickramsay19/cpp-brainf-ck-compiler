#include "ast.hpp"
#include <memory>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include "emitter.hpp"

/*std::ostream& operator<<(std::ostream& os, const std::unique_ptr<AST> ast) {
    if (ast) {
        os << static_cast<std::string>(*ast);
    } else {
        os << "nullptr";
    }
    return os;
}*/

void Program::accept(CodeGenVisitor& visitor) const {
    visitor.visit(this);
}

void FullStmtList::accept(CodeGenVisitor& visitor) const {
    stmt->accept(visitor);
    next->accept(visitor); // recurse
}

FullStmtList::operator std::string() const {
    std::stringstream ss;
    ss << "FullStmtList(" << static_cast<std::string>(*stmt) << "," << static_cast<std::string>(*next) << ")";
    return ss.str();
}

void EmptyStmtList::accept(CodeGenVisitor& visitor) const {}

void LeftStmt::accept(CodeGenVisitor& visitor) const {
    visitor.visit(*this);
}

void RightStmt::accept(CodeGenVisitor& visitor) const {
    visitor.visit(*this);
}

RightStmt::operator std::string() const {
    return ">";
}

void IncStmt::accept(CodeGenVisitor& visitor) const {
    visitor.visit(*this);
}

void DecStmt::accept(CodeGenVisitor& visitor) const {
    visitor.visit(*this);
}

void PrintStmt::accept(CodeGenVisitor& visitor) const {
    visitor.visit(*this);
}

void LoopStmt::accept(CodeGenVisitor& visitor) const {
    visitor.visit(*this);
}

LoopStmt::operator std::string() const {
    std::stringstream ss;
    ss << "LoopStmt(" << static_cast<std::string>(*stmt_list) << ")";
    return ss.str();
}
