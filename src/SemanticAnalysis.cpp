#include "ast.h"

#include <iostream>
#include <cstdlib>

bool validateExpression(SymbolTable *table, Expression *e, int startLine, int endLine);


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

    if (!checker) std::cerr << "ScopeError::The scope of the variable " << *varName << " is invalid!" << std::endl;

    return checker;
}

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

bool validateTerm(SymbolTable *table, Term *t, int startLine, int endLine) {
    bool b = true;

    if (t->t1 != NULL) {
        Factor *factor = t->t1->f;
        b = validateFactor(table, factor, startLine, endLine);
    } else if (t->t2 != NULL) {
        Factor *factor1 = t->t2->f1;
        Factor *factor2 = t->t2->f2;
        b = validateFactor(table, factor1, startLine, endLine);
        b = b && validateFactor(table, factor2, startLine, endLine);
    } else {
        b = false;
    }

    return b;
}

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

bool checkScopeOfParamsForFunctionCall(SymbolTable *table, Call *call, vector<Variable *> *parameters, int startLine, int endLine) {
    if (parameters != NULL) {
        // add parameters to symbol table
        int numOfParams = parameters->size();

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

    for (int i = 0; i < size; i++) {
        Expression *e = params->at(i);
        result = result && validateExpression(table, e, startLine, endLine);
    }

    if (parameters != NULL) {
        int numOfParams = parameters->size();

        // remove parameters from symbol table
        table->remove(numOfParams);

        //TODO need to test function call!!
        std::cout << "numOfParams = " << numOfParams << " , size = " << size << std::endl;
    }

    return result;
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
        for (int i = 0; i < size; i++) {
            Stmt *stmt = stmts->at(i);
            std::string *varName = NULL;

            switch (stmt->type) {
                case 'v':
                    varName = stmt->statement->v->v->id->i;
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
                    //TODO
                    break;
                case 'a':
                    varName = stmt->statement->a->i->i;
                    checker = checker && checkScopeOfVarName(table, varName, params, startLine, endLine);
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
