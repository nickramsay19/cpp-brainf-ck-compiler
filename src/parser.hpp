#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <memory>
#include <utility>
#include <stdexcept>
#include "ast.hpp"
#include "debug_print.hpp"

class Parser {
public:
    Parser(std::fstream file) {
        file_.swap(file);
    }

    std::unique_ptr<Program> parse() {
        next();
        DEBUG_COUT << "parse \"" << c_ << "\"\n";

        std::unique_ptr<StmtList> stmt_list = parseStmtList();
        
        return std::unique_ptr<Program>(new Program(std::move(stmt_list)));
    }

protected:
    char next() {
        c_ = file_.get();
        return c_;
    }

    void accept(char c) {
        if (c_ != c) {
            throw std::runtime_error("Unexpected character");
        }
        next();
    }

    std::unique_ptr<StmtList> parseStmtList() {
        DEBUG_COUT << "parseStmtList \"" << c_ << "\"\n";

        while (!file_.eof()) {
            switch(c_) {
            case ']':
                return std::unique_ptr<StmtList> (new EmptyStmtList());
            case '+':
            case '-':
            case '<':
            case '>':
            case '.':
            case ',': 
            case '[':
                return parseFullStmtList();
            default:
                break;
            }

            next();
        }

        return std::unique_ptr<StmtList> (new EmptyStmtList());
    }

    std::unique_ptr<StmtList> parseFullStmtList() {
        DEBUG_COUT << "parseFullStmtList \"" << c_ << "\"\n";

        std::unique_ptr<Stmt> stmt = parseStmt();
        if (!stmt) {
            throw std::runtime_error("Failed to parse Stmt while parsing a StmtList");
        }

        std::unique_ptr<StmtList> stmt_list = parseStmtList();
        if (!stmt_list) {
            throw std::runtime_error("Failed to parse StmtList (next) while parsing a StmtList");
        }

        return std::unique_ptr<StmtList> (new FullStmtList(std::move(stmt), std::move(stmt_list)));
    }

    std::unique_ptr<Stmt> parseStmt() {
        DEBUG_COUT << "parseStmt \"" << c_ << "\"\n";

        switch (c_) {
        case '+':
            next();
            return std::unique_ptr<Stmt> (new IncStmt()); 
        case '-':
            next();
            return std::unique_ptr<Stmt> (new DecStmt());
        case '<':
            next();
            return std::unique_ptr<Stmt> (new LeftStmt()); 
        case '>':
            next();
            return std::unique_ptr<Stmt> (new RightStmt());
        /*case ',':
            return std::unique_ptr<Stmt> (new ReadStmt()); */
        case '.':
            next();
            return std::unique_ptr<Stmt> (new PrintStmt()); 
        case '[':
            return parseLoopStmt();
        case ']': 
            throw std::runtime_error("Unexpected \"]\" token");
        default:
            //throw std::runtime_error("Unexpected token");
            next();
            return std::unique_ptr<Stmt>();
        }
    }

    std::unique_ptr<Stmt> parseLoopStmt() {
        DEBUG_COUT << "parseLoopStmt \"" << c_ << "\"\n";

        accept('[');

        std::unique_ptr<StmtList> stmt_list = parseStmtList();
        if (!stmt_list) {
            throw std::runtime_error("Failed to parse StmtList while parsing a LoopStmt");
        }

        accept(']');

        return std::unique_ptr<Stmt> (new LoopStmt(std::move(stmt_list)));
    }

    std::fstream file_;
    char c_;
};

