#include "ast.h"

#include <iostream>
#include <cstdlib>


bool checkScopeOfVarName(SymbolTable *table, std::string *varName, vector<Var *> *params, int startLine, int endLine) {
    bool checker = false;

    // validate the parameters
    if (varName == NULL || startLine < 0) {
        std::cout << "Error::Invalid parameter - checkScopeOfVarName()" << std::endl;
        return checker;
    }

    // check if the params is NULL
    if (params != NULL) {
        int size = params->size();

        for (int i = 0; i < size; i++) {
            //TODO
        }
    }

    // check if the given varName is found
    if (!checker) {
        vector<VarInfo *> *list = table->getList();
        int size = list->size();

        for (int i = 0; i < size; i++) {
            //TODO
        }
    }

    return checker;
}


/**
 * Check the variable scopes of all statements for the semantic analysis.
 * @param {table} Pointer that points the Symbol Table.
 * @param {procs} Pointer that points the list of Procedure instances.
 * @return If all statements are sematically valid, returns true. Otherwise, returns false.
 */
bool checkScopeOfStmts(SymbolTable *table, vector<Stmt *> *stmts, vector<Var *> *params, int startLine, int endLine) {
    bool checker = true;
    int size = stmts->size();

    // use try-catch statement to catch possible error
    try {
        for (int i = 0; i < size; i++) {
            Stmt *stmt = stmts->at(i);
            std::string *varName = NULL;

            switch (stmt->type) {
                case 'v':
                    varName = stmt->statement->v->id->i;
                    checker = checker && checkScopeOfVarName(table, varName, params, startLine, endLine);
                    break;
                case 'p':
                    break;
                case 'g':
                    //TODO Var -> Assign ? Id ?
                    checker = checker && checkScopeOfVarName(table, varName, params, startLine, endLine);
                    break;
                case 'w':
                    checker = checker && checkScopeOfStmts(table, stmt->statement->w->s1->stmts, params, stmt->statement->w->s1->startLine, stmt->statement->w->s1->endLine);
                    break;
                case 'i':
                    //TODO
                    break;
                case 'a':
                    varName = stmt->statement->a->i->i;
                    checker = checker && checkScopeOfVarName(table, varName, params, startLine, endLine);
                    break;
                case 'c':
                    //TODO parameters -> check scope
                    break;
                default:
                    std::cerr << "Error::Invalid statement type!" << std::endl;
                    std::exit(EXIT_FAILURE);
            }

        }
    } catch (const std::out_of_range & ex) {
        std::cerr << "out_of_range Exception Caught :: " << ex.what() << std::endl;
        std::exit(EXIT_FAILURE);
    }

    return checker;
}


/**
 * Check the variable scopes of the given procedure for the semantic analysis.
 * @param {table} Pointer that points the Symbol Table.
 * @param {procs} Pointer that points the list of Procedure instances.
 * @return If the procedure is sematically valid, returns true. Otherwise, returns false.
 */
bool checkScopeOfProcedure(SymbolTable *table, Procedure *proc) {
    vector<Var *> *params = proc->params;
    vector<Stmt *> *stmts = proc->cs->stmts;
    return checkScopeOfStmts(table, stmts, params, proc->startLine, proc->endLine);
}


/**
 * Check the variable scopes of all procedures for the semantic analysis.
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
