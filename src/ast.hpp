#pragma once
#include <memory>
#include <utility>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>

class CodeGenVisitor;
class StmtList;
class Stmt;

class AST {
public:
    virtual void accept(CodeGenVisitor& visitor) const {}
};

struct Program : public AST {
    explicit Program(std::unique_ptr<StmtList> stmt_list) : stmt_list{std::move(stmt_list)} {}

    void accept(CodeGenVisitor& visitor) const override;

    std::unique_ptr<StmtList> stmt_list;
};

// StmtList
struct StmtList : public AST {
    //virtual void accept(CodeGenVisitor& visitor) const = default;
    //virtual ~StmtList() {}
    virtual ~StmtList() = default;
};

struct FullStmtList : public StmtList {
    explicit FullStmtList(std::unique_ptr<Stmt> stmt, std::unique_ptr<StmtList> next) : 
        stmt{std::move(stmt)},
        next{std::move(next)} {}

    void accept(CodeGenVisitor& visitor) const override;

    std::unique_ptr<Stmt> stmt;
    std::unique_ptr<StmtList> next;
};

struct EmptyStmtList : public StmtList {
    void accept(CodeGenVisitor& visitor) const override;
};

// Stmt
struct Stmt : public AST {
    virtual ~Stmt() = default;
};

struct LeftStmt : public Stmt {
    void accept(CodeGenVisitor& visitor) const override;
};

struct RightStmt : public Stmt {
    void accept(CodeGenVisitor& visitor) const override;
};

struct IncStmt : public Stmt {
    void accept(CodeGenVisitor& visitor) const override;
};

/*struct DecStmt : public Stmt {
    void accept(CodeGenVisitor& visitor) const override;
};

struct ReadStmt : public Stmt {
    void accept(CodeGenVisitor& visitor) const override;
};*/

struct PrintStmt : public Stmt {
    void accept(CodeGenVisitor& visitor) const override;
};

/*struct LoopStmt : public Stmt {
    LoopStmt(std::unique_ptr<StmtList> stmt_list) : stmt_list{std::move(stmt_list)} {}

    void accept(CodeGenVisitor& visitor) const override;

    std::unique_ptr<StmtList> stmt_list;
};*/
