#pragma once
#include <memory>
#include <string>
#include <sstream>
#include <utility>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>

class Generator;
class StmtList;
class Stmt;

class AST {
public:
    virtual void accept(Generator& visitor) const {}

    virtual operator std::string() const = 0;

    //friend std::ostream& operator<<(std::ostream& os, const std::unique_ptr<AST> ast);
};

//std::ostream& operator<<(std::ostream& os, const std::unique_ptr<AST> ast);

// Stmt
struct Stmt : public AST {
    virtual ~Stmt() = default;
};

struct LeftStmt : public Stmt {
    void accept(Generator& visitor) const override;

    operator std::string() const override {
        return "<";
    }
};

struct RightStmt : public Stmt {
    void accept(Generator& visitor) const override;

    operator std::string() const override;
};

struct IncStmt : public Stmt {
    void accept(Generator& visitor) const override;

    operator std::string() const override {
        return "+";
    }
};

struct DecStmt : public Stmt {
    void accept(Generator& visitor) const override;

    operator std::string() const override {
        return "-";
    }
};

struct ReadStmt : public Stmt {
    void accept(Generator& visitor) const override;

    operator std::string() const override {
        return ",";
    }
};

struct PrintStmt : public Stmt {
    void accept(Generator& visitor) const override;

    operator std::string() const override {
        return ".";
    }
};

struct LoopStmt : public Stmt {
    LoopStmt(std::unique_ptr<StmtList> stmt_list) : stmt_list{std::move(stmt_list)} {}

    void accept(Generator& visitor) const override;

    operator std::string() const override;

    std::unique_ptr<StmtList> stmt_list;
};

struct StmtList : public AST {
    virtual ~StmtList() = default;
};

struct FullStmtList : public StmtList {
    explicit FullStmtList(std::unique_ptr<Stmt> stmt, std::unique_ptr<StmtList> next) : 
        stmt{std::move(stmt)},
        next{std::move(next)} {}

    void accept(Generator& visitor) const override;

    operator std::string() const override;

    std::unique_ptr<Stmt> stmt;
    std::unique_ptr<StmtList> next;
};

struct EmptyStmtList : public StmtList {
    void accept(Generator& visitor) const override;

    operator std::string() const override {
        return "EmptyStmtList";
    }
};

struct Program : public AST {
    explicit Program(std::unique_ptr<StmtList> stmt_list) : stmt_list{std::move(stmt_list)} {}

    void accept(Generator& visitor) const override;

    operator std::string() const override {
        std::stringstream ss;
        ss << "Program(" << static_cast<std::string>(*stmt_list) << ")";
        return ss.str();
    }

    std::unique_ptr<StmtList> stmt_list;
};
