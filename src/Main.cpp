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

    vector<Procedure *> *procedures = program->ps;
    int size = procedures->size();

    // use for loop to iterate procedures
    for (int i = 0; i < size; i++) {
        Procedure *p = procedures->at(i);

        //TODO insert parameters into the symbol table, and remove them from table after checking.

        // run the type checker for each procedure
        if (!checkType(table, p->cs->stmts)) {
            std::cerr << "Error::Type checking failed - " << *(p->name) << std::endl;
            return 0;
        }
    }

    vector<TACList *> list;

    list.push_back(generateTACList(table, program->cs->stmts));

    for (int i = 0; i < size; i++) {
        Procedure *p = procedures->at(i);
        list.push_back(generateTACList(table, p->cs->stmts));
    }

    //TODO generate file for TAC

    //TODO part4

    delete table;
    std::cout << "Program ends" << std::endl;
    return 1;
}
