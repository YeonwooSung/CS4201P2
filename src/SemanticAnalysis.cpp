#include "ast.h"

#include <iostream>
#include <cstdlib>

bool validateExpression(SymbolTable *table, Expression *e, int startLine, int endLine);


/**
 * Check the scope of the variable by comparing the declared line of the variable and the start line and end line.
 * When this function checks the scope of the variable, it will first check if the given variable is a parameter of the current procedure.
 * If not, it will check if the given variable is declared before by comparing the the declared line with the start line and end line
 * of the current procedure (or compound).
 * By doing this, we could detect the variable scoping error.
 *
 * @param {table} the symbol table
 * @param {varName} the name of the variable that should be checked
 * @param {params} the list of parameters of the procedure (the main procedure does not have parameters, so it will be NULL if it is in the main procedure)
 * @param {startLine} the startLine of the procedure
 * @param {endLine} the endLine of the current compound
 * @return If there is no errror, returns true. Otherwise, returns false.
 */
bool checkScopeOfVarName(SymbolTable *table, std::string *varName, vector<Variable *> *params, int startLine, int endLine) {
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
            Variable *param = params->at(i);
            string *paramName = param->v->id->i;
            int res = paramName->compare(*varName);

            if (res == 0) {
                checker = true;
                break;
            }
        }
    }

    // check if the given varName is found
    if (!checker) {
        vector<VarInfo *> *list = table->getList();
        int size = list->size();

        for (int i = 0; i < size; i++) {
            VarInfo *info = list->at(i);
            string *name = info->name;
            int res = name->compare(*varName);

            if (res == 0) {
                int declaredLine = info->declaredLine;

                // check if the declared line is in the current scope
                if (startLine <= declaredLine && endLine >= declaredLine) {
                    checker = true;
                    break;
                }
            }
        }
    }

    /*
     * The value of the checker is true if the scope of the variable is valid.
     * So, we negate the checker to check if there is variable scoping error.
     */
    if (!checker) std::cerr << "ScopeError::The scope of the variable " << *varName << " is invalid!" << std::endl;

    return checker;
}

/**
 * This function analyses the given factor to check the variable scoping error.
 * @param {table} the symbol table
 * @param {f} The factor that should be analysed
 * @param {startLine} the startLine of the procedure
 * @param {endLine} the endLine of the current compound
 * @return If there is no error, returns true. Otherwise, returns false.
 */
bool validateFactor(SymbolTable *table, Factor *f, int startLine, int endLine) {
    bool b = false;

    if (f->con != NULL) {
        b = true;
    } else if (f->e != NULL) {
        b = validateExpression(table, f->e, startLine, endLine);
    } else if (f->id != NULL) {
        b = checkScopeOfVarName(table, f->id->i, NULL, startLine, endLine);
    } else if (f->note != NULL) {
        b = validateExpression(table, f->note, startLine, endLine);
    }

    return b;
}

/**
 * This function analyses the given term to check the variable scoping error.
 * @param {table} the symbol table
 * @param {t} The term that should be analysed
 * @param {startLine} the startLine of the procedure
 * @param {endLine} the endLine of the current compound
 * @return If there is no error, returns true. Otherwise, returns false.
 */
bool validateTerm(SymbolTable *table, Term *t, int startLine, int endLine) {
    bool b = true;

    // check which type is currently used.
    if (t->t1 != NULL) {
        Factor *factor = t->t1->f;
        b = validateFactor(table, factor, startLine, endLine); //do the semantic analysis by checking the factor in the term
    } else if (t->t2 != NULL) {
        Factor *factor1 = t->t2->f1;
        Factor *factor2 = t->t2->f2;

        //do the semantic analysis by checking the factors in the term

        b = validateFactor(table, factor1, startLine, endLine);
        b = b && validateFactor(table, factor2, startLine, endLine);
    } else {
        b = false;
    }

    return b;
}

/**
 * This function analyses the given simple expression to check the variable scoping error.
 * @param {table} the symbol table
 * @param {s} The simple expression that should be analysed
 * @param {startLine} the startLine of the procedure
 * @param {endLine} the endLine of the current compound
 * @return If there is no error, returns true. Otherwise, returns false.
 */
bool validateSimple(SymbolTable *table, Simple *s, int startLine, int endLine) {
    bool b = true;

    if (s->e1 != NULL) {
        Term *term = s->e1->t;
        b = validateTerm(table, term, startLine, endLine);
    } else if (s->e2 != NULL) {
        Simple2 *s2 = s->e2;

        Term *term1 = s2->t1;
        Term *term2 = s2->t2;

        b = validateTerm(table, term1, startLine, endLine);
        b = b && validateTerm(table, term2, startLine, endLine);
    } else {
        b = false;
    }

    return b;
}

/**
 * This function analyses the given expression to check the variable scoping error.
 * @param {table} the symbol table
 * @param {e} The expression that should be analysed
 * @param {startLine} the startLine of the procedure
 * @param {endLine} the endLine of the current compound
 * @return If there is no error, returns true. Otherwise, returns false.
 */
bool validateExpression(SymbolTable *table, Expression *e, int startLine, int endLine) {
    char type = e->type;
    bool b = true;

    if (type != 'e') {
        Simple *s = e->expr->e1;
        b = validateSimple(table, s, startLine, endLine);
    } else {
        Expr2 *expr = e->expr->e2;
        b = validateSimple(table, expr->e1, startLine, endLine);
        b = b && validateSimple(table, expr->e2, startLine, endLine);
    }

    return b;
}

/**
 * This function performs the semantic analysis for the function call statement.
 * @param {table} the symbol table
 * @param {call} the function call statement
 * @param {parameters} the list of parameters of the procedure (the main procedure does not have parameters, so it will be NULL if it is in the main procedure)
 * @param {startLine} the startLine of the procedure
 * @param {endLine} the endLine of the current compound
 * @return If there is no error, returns true. Otherwise, returns false.
 */
bool checkScopeOfParamsForFunctionCall(SymbolTable *table, Call *call, vector<Variable *> *parameters, int startLine, int endLine) {
    if (parameters != NULL) {
        // add parameters to symbol table
        int numOfParams = parameters->size();

        // use for loop to check all parameters
        for (int i = 0; i < numOfParams; i++) {
            VarInfo *info = new VarInfo;
            info->declaredLine = startLine;
            info->name = parameters->at(i)->v->id->i;

            table->addVarInfo(info);
        }
    }

    vector<Expression *> *params = call->params;
    int size = params->size();
    bool result = true;

    // use for loop to iterate the parameters
    for (int i = 0; i < size; i++) {
        Expression *e = params->at(i);
        result = result && validateExpression(table, e, startLine, endLine);
    }

    // check if the parameters of the procedure is given
    if (parameters != NULL) {
        int numOfParams = parameters->size();

        // remove parameters from symbol table
        table->remove(numOfParams);
    }

    return result;
}

/**
 * Parse the If statement to check the variable scoping errors, and validate the expression and statements in the if statement.
 * @param {table} the symbol table
 * @param {stmt} the If statement
 * @param {params} the list of parameters of the procedure (the main procedure does not have parameters, so it will be NULL if it is in the main procedure)
 * @param {startLine} the startLine of the procedure
 * @param {endLine} the endLine of the current compound
 * @return If there is no errror, returns true. Otherwise, returns false.
 */
bool doSemanticAnalysisForIfStatement(SymbolTable *table, If *stmt, vector<Variable *> *params, int startLine, int endLine) {
    bool checker = false;

    // check whether If1 is used for the If statement (otherwise, If2 is used).
    if (stmt->type != '1') {
        If2 *if2 = stmt->i->if2;
        checker = checker && validateExpression(table, if2->e, startLine, (if2->s1->startLine + 1));
        checker = checker && checkScopeOfStmts(table, if2->s1->stmts, NULL, startLine, if2->s1->endLine);
        checker = checker && checkScopeOfStmts(table, if2->s2->stmts, NULL, startLine, if2->s2->endLine);
    } else {
        If1 *if1 = stmt->i->if1;
        checker = checker && validateExpression(table, if1->e, startLine, (if1->s->startLine + 1));
        checker = checker && checkScopeOfStmts(table, if1->s->stmts, NULL, startLine, if1->s->endLine);
    }

    return checker;
}

/**
 * Check the variable scopes of all statements for the semantic analysis.
 * @param {table} Pointer that points the Symbol Table.
 * @param {procs} Pointer that points the list of Procedure instances.
 * @return If all statements are sematically valid, returns true. Otherwise, returns false.
 */
bool checkScopeOfStmts(SymbolTable *table, vector<Stmt *> *stmts, vector<Variable *> *params, int startLine, int endLine) {
    bool checker = true;
    int size = stmts->size();

    // use try-catch statement to catch possible error
    try {
        // use for loop to iterate the list of statements
        for (int i = 0; i < size; i++) {
            Stmt *stmt = stmts->at(i);
            std::string *varName = NULL;

            // use switch statement to check the type of the current statement
            switch (stmt->type) {
                case 'v':
                    // check if this Var contains either Assign or Id
                    if (stmt->statement->v->type != 'i') {
                        varName = stmt->statement->v->v->a->i->i;
                        checker = checker && validateExpression(table, stmt->statement->v->v->a->e, startLine, endLine);
                    } else {
                        varName = stmt->statement->v->v->id->i;
                    }

                    checker = checker && checkScopeOfVarName(table, varName, params, startLine, endLine);
                    break;
                case 'p':
                    break;
                case 'g':
                    varName = stmt->statement->g->id->i;
                    checker = checker && checkScopeOfVarName(table, varName, params, startLine, endLine);
                    break;
                case 'w':
                    checker = checker && checkScopeOfStmts(table, stmt->statement->w->s1->stmts, params, startLine, stmt->statement->w->s1->endLine);
                    break;
                case 'i':
                    // check if this If union contains either If1 or If2
                    if (stmt->type != '1') {
                        If2 *if2 = stmt->statement->i->i->if2;
                        checker = checker && validateExpression(table, if2->e, startLine, (if2->s1->startLine + 1));
                        checker = checker && checkScopeOfStmts(table, if2->s1->stmts, NULL, startLine, if2->s1->endLine);
                        checker = checker && checkScopeOfStmts(table, if2->s2->stmts, NULL, startLine, if2->s2->endLine);
                    } else {
                        If1 *if1 = stmt->statement->i->i->if1;
                        checker = checker && validateExpression(table, if1->e, startLine, (if1->s->startLine + 1));
                        checker = checker && checkScopeOfStmts(table, if1->s->stmts, NULL, startLine, if1->s->endLine);
                    }
                    break;
                case 'a':
                    varName = stmt->statement->a->i->i;
                    checker = checker && checkScopeOfVarName(table, varName, params, startLine, endLine);
                    checker = checker && validateExpression(table, stmt->statement->a->e, startLine, endLine);
                    break;
                case 'c':
                    checker = checker && checkScopeOfParamsForFunctionCall(table, stmt->statement->c, params, startLine, endLine);
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
    vector<Variable *> *params = proc->params;
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
