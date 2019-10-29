#include "ast.h"

#include <iostream>
#include <cstdlib>


int main() {
    SymbolTable *table = new SymbolTable();
    Program *program = getExampleTree1(table); //get the AST

    if (!checkScopeOfAllProcedures(table, program->ps)) {
        std::cerr << "Error::Semantic analysis failed" << std::endl;
        return 0;
    }

    //TODO

    table->~SymbolTable();
    std::cout << "Program ends" << std::endl;
    return 1;
}
