#include "ast.h"

#include <iostream>
#include <cstdlib>


bool checkScopeOfProcedure(SymbolTable *table, Procedure *proc) {
    bool checker = true;
    vector<Var *> *params = proc->params;
    vector<Statement *> *stmts = proc->cs->stmts;
    int size = stmts->size();

    // use try-catch statement to catch possible error
    try {
        for (int i = 0; i < size; i++) {
            Statement *stmt = stmts->at(i);

            //TODO
        }
    } catch (const std::out_of_range & ex) {
        std::cerr << "out_of_range Exception Caught :: " << ex.what() << std::endl;
        std::exit(EXIT_FAILURE);
    }

    return checker;
}

/**
 * Check the scope of all procedures for the semantic analysis.
 * @param {table} Pointer that points the Symbol Table.
 * @param {procs} Pointer that points the list of Procedure instances.
 * @return If all procedures are sematically valid, returns true. Otherwise, returns false.
 */
bool checkScopeOfAllProcedures(SymbolTable *table, vector<Procedure *> *procs) {
    int size = procs->size();
    bool checker = true;

    // use try-catch statement to catch possible error
    try {
        for (int i = 0; i < size; i++) {
            Procedure *proc = procs->at(i);
            bool res = checkScopeOfProcedure(table, proc);
            checker = checker && res;
        }
    } catch (const std::out_of_range & ex) {
        std::cerr << "out_of_range Exception Caught :: " << ex.what() << std::endl;
        std::exit(EXIT_FAILURE);
    }

    return checker;
}
