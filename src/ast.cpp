#include "ast.hpp"
#include <memory>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include "generator.hpp"

/*std::ostream& operator<<(std::ostream& os, const std::unique_ptr<AST> ast) {
    if (ast) {
        os << static_cast<std::string>(*ast);
    } else {
        os << "nullptr";
    }
    return os;
}*/

void Program::accept(Generator& generator) const {
    generator.visit(this);
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

void LeftStmt::accept(Generator& generator) const {
    generator.visit(*this);
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

void DecStmt::accept(Generator& generator) const {
    generator.visit(*this);
}

void PrintStmt::accept(Generator& generator) const {
    generator.visit(*this);
}

void LoopStmt::accept(Generator& generator) const {
    generator.visit(*this);
}

LoopStmt::operator std::string() const {
    std::stringstream ss;
    ss << "LoopStmt(" << static_cast<std::string>(*stmt_list) << ")";
    return ss.str();
}
