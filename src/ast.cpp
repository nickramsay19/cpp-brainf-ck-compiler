#include "ast.hpp"
#include <memory>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include "generator.hpp"

void Program::accept(Generator& generator) const {
    generator.visit(this);
}

Program::operator std::string() const {
    std::stringstream ss;
    ss << "Program(" << static_cast<std::string>(*stmt_list) << ")";
    return ss.str();
}

void FullStmtList::accept(Generator& generator) const {
    stmt->accept(generator);
    next->accept(generator); // recurse
}

FullStmtList::operator std::string() const {
    std::stringstream ss;
    ss << "FullStmtList(" << static_cast<std::string>(*stmt) << "," << static_cast<std::string>(*next) << ")";
    return ss.str();
}

void EmptyStmtList::accept(Generator& generator) const {}

EmptyStmtList::operator std::string() const {
    return "EmptyStmtList";
}

void LeftStmt::accept(Generator& generator) const {
    generator.visit(*this);
}

LeftStmt::operator std::string() const {
    return "<";
}

void RightStmt::accept(Generator& generator) const {
    generator.visit(*this);
}

RightStmt::operator std::string() const {
    return ">";
}

void IncStmt::accept(Generator& generator) const {
    generator.visit(*this);
}

IncStmt::operator std::string() const {
    return "+";
}

void DecStmt::accept(Generator& generator) const {
    generator.visit(*this);
}

DecStmt::operator std::string() const {
    return "-";
}

void PrintStmt::accept(Generator& generator) const {
    generator.visit(*this);
}

PrintStmt::operator std::string() const {
    return ".";
}

void LoopStmt::accept(Generator& generator) const {
    generator.visit(*this);
}

LoopStmt::operator std::string() const {
    std::stringstream ss;
    ss << "LoopStmt(" << static_cast<std::string>(*stmt_list) << ")";
    return ss.str();
}
