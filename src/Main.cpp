#include "ast.h"
#include <iostream>


int main() {
    SymbolTable *table = new SymbolTable();
    Program *program = getExampleTree1(table); //get the AST

    // run the semantic analysis
    if (!checkScopeOfAllProcedures(table, program->ps) || !checkScopeOfStmts(table, program->cs->stmts, NULL, program->cs->startLine, program->cs->endLine)) {
        std::cerr << "Error::Semantic analysis failed" << std::endl;
        return 0;
    }

    // run the type checking
    if (!checkType(table, program->cs->stmts)) {
        std::cerr << "Error::Type checking failed - Main method" << std::endl;
        return 0;
    }

    //TODO

    table->~SymbolTable();
    std::cout << "Program ends" << std::endl;
    return 1;
}
