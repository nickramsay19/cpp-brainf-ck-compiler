#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <memory>
#include <utility>
#include <stdexcept>
#include "ast.hpp"

class Parser {
public:
    Parser(std::fstream file) : file{file} {}

    Program parse() {
        return Program(std::make_unique<Stmt>(parseStmtList()));
    }

protected:
    StmtList parseStmtList() {
        switch (file.get()) {
        case ']':
        case -1:
            return EmptyStmtList();
        default:
            return FullStmtList(std::make_unique<Stmt>(parseStmt()), std::make_unique<StmtList>(parsestmtList()));
        {
    }

    Stmt parseStmt() {
        switch (file.get()) {
        case ']':
        case -1:
            std::cerr << "Unexpected token\n";
            throw std::runtime_error("Unexpected token");
        case '+':
            return IncStmt();
        case '-':
            return DecStmt();
        case '<':
            return LeftStmt();
        case '>':
            return RightStmt();
        case '.':
            return PrintStmt();
        default:
            // assume its a comment
            return parseStmt(); // try again
        }
    }

    std::fstream file;
};

