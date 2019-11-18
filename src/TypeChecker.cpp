#include "ast.h"
#include <iostream>

char getType(SymbolTable *table, Expression *e);
bool checkType(SymbolTable *table, Expression *expression);


char getType(SymbolTable *table, Factor * f) {
    if (f->con != NULL) {
        string *con = f->con;

        // check if the constant is boolean
        if (con->compare("True") == 0 || con->compare("False") == 0) {
            return 'b'; //'b' for boolean
        } else {
            string::const_iterator it = con->begin();

            // to check if the value of the constant is the negative integer
            if (con->at(0) == '-') {
                it += 1;
            }

            // use while loop to iterate the iterator
            while (it != con->end() && std::isdigit(*it)) it += 1;

            if (!con->empty() && it == con->end()) {
                return 'i'; //'i' for int
            } else {
                return 's'; //'s' for string
            }
        }
    } else if (f->e != NULL) {
        return getType(table, f->e);
    } else if (f->id != NULL) {
        Id *id = f->id;
        VarInfo *info = table->getVarInfo(id->i);
        return info->type;
    } else {
        return getType(table, f->note);
    }
}

char getType(SymbolTable *table, Term *t) {
    if (t == NULL) return -1;

    // check which attribute is not NULL
    if (t->t1 != NULL) {
        Factor *f = t->t1->f;
        return getType(table, f);
    } else {
        Term2 *t2 = t->t2;
        MulOp *mulop = t2->op;
        Factor *f1 = t2->f1;
        Factor *f2 = t2->f2;

        char type1 = getType(table, f1);
        char type2 = getType(table, f2);

        if (type1 == 0) {
            return type2;
        }

        if (type2 == 0) {
            return type1;
        }

        // compare the type of 2 Factors
        if (type1 != type2) {
            return -1;
        } else {

            // check if the MulOp is And
            if (*mulop != And) {
                return (type1 == 'i' ? 'i' : -1);
            } else {
                return (type1 == 'b' ? 'b' : -1);
            }
        }
    }
}

char getType(SymbolTable *table, Simple *s) {
    if (s == NULL) return -1;

    // check which attribute is not NULL
    if (s->e1 != NULL) {
        Term *t = s->e1->t;
        return getType(table, t);
    } else {
        // Since this expression contains the Add operator, all both term1 and term2 should be an integer.
        // So, this method will check the type of 2 terms, and return -1 if the type of the term is not an integer.

        AddOp *addop = s->e2->op;
        Term *t1 = s->e2->t1;
        Term *t2 = s->e2->t2;

        char type1 = getType(table, t1);
        char type2 = getType(table, t2);

        /* check the returned value */

        if (type1 == 0) {
            return type2;
        }

        if (type2 == 0) {
            return type1;
        }

        // compare the types of 2 Terms
        if (type1 != type2) {
            return -1;
        } else {
            // check if the AddOp is Or
            if (*addop != Or) {
                return (type1 == 'i' ? 'i' : -1);
            } else {
                return (type1 == 'b' ? 'b' : -1);
            }
        }
    }
}

char getType(SymbolTable *table, Expression *e) {
    if (e == NULL) return -1;

    if (e->type != 's') {
        Expr2 *expr2 = e->expr->e2;
        RelOp *relop = expr2->op;
        Simple *s1 = expr2->e1;
        Simple *s2 = expr2->e2;

        char type1 = getType(table, s1);
        char type2 = getType(table, s2);

        if (type1 == 0) {
            return type2;
        }

        if (type2 == 0) {
            return type1;
        }

        // compare types of 2 simple expressions
        if (type1 != type2) {
            return -1;
        } else {
            return (type1 == 'i' ? 'i' : -1);
        }
    } else {
        // call the function to get the type of simple expression.
        return getType(table, e->expr->e1);
    }
}

bool checkType(SymbolTable *table, Factor *f) {
    if (f->e != NULL) {
        return checkType(table, f->e);
    } else if (f->note != NULL) {
        return checkType(table, f->note);
    }

    return true;
}

bool checkType(SymbolTable *table, Term *t) {
    if (t == NULL) return false;

    bool checker = true;

    // check if the pointer is not null
    if (t->t1 != NULL) {
        Factor *f = t->t1->f;
        checker = checkType(table, f); //check the type of Factor
    } else {
        Term2 *t2 = t->t2;
        MulOp *mulop = t2->op;
        Factor *f1 = t2->f1;
        Factor *f2 = t2->f2;

        // Since the given Term contains the MulOp, both factor1 and factor2 should be an integer.
        // Thus, this method will check the type of factors, and return suitable boolean value.

        char type1 = getType(table, f1);
        char type2 = getType(table, f2);

        if (type1 == 0) {
            if (type2 != -1) return true;

            return false;
        }

        if (type2 == 0) {
            if (type1 != -1) return true;

            return false;
        }

        // compare the types of 2 Factors
        if (type1 != type2) {
            checker = false;
        } else {
            // check if the MulOp is And
            if (*mulop != And) {
                checker = type1 == 'i';
            } else {
                checker = type1 == 'b';
            }
        }
    }

    return checker;
}

bool checkType(SymbolTable *table, Simple *s) {
    if (s == NULL) return false;

    bool checker = true;

    if (s->e1 != NULL) {
        checker = checkType(table, s->e1->t);
    } else {
        Simple2 *s2 = s->e2;
        AddOp *addop = s2->op;
        Term *t1 = s2->t1;
        Term *t2 = s2->t2;

        // Since the given expression contains the AddOp, both factor1 and factor2 should be an integer.
        // Thus, this method will check the type of factors, and return suitable boolean value.

        char type1 = getType(table, t1);
        char type2 = getType(table, t2);

        if (type1 == 0 && type2 == 0) {
            return true;
        }

        if (type1 == 0) {

            if (*addop != Or) {
                return (type2 == 'i');
            } else {
                return (type2 == 'b');
            }
        }

        if (type2 == 0) {
            if (*addop != Or) {
                return (type1 == 'i');
            } else {
                return (type1 == 'b');
            }
        }

        // compare types of 2 Terms
        if (type1 != type2) {
            checker = false;
        } else {
            if (*addop != Or) {
                checker = (type1 == 'i');
            } else {
                checker = (type1 == 'b');
            }
        }
    }

    return checker;
}

bool checkType(SymbolTable *table, Expression *expression) {
    if (expression == NULL) return false;

    bool checker = true;

    // Check if the Expression contains Expr2 or Simple
    if (expression->type != 's') {
        Expr2 *expr2 = expression->expr->e2;
        Simple *s1 = expr2->e1;
        Simple *s2 = expr2->e2;

        // The Expr2 has relational operator, thus, the type of both s1 and s2 should be integer.

        char type1 = getType(table, s1);
        char type2 = getType(table, s2);

        // compare types of 2 simple expressions
        if (type1 != type2) {
            if ((type1 == 0 || type2 == 0) && (type1 == 'i' || type2 == 'i')) {
                checker = true;
            } else {
                checker = false;
            }

        } else {
            checker = (type1 == 'i' || type1 == 0);
        }
    } else {
        Simple *simple = expression->expr->e1;
        checker = checkType(table, simple);
    }

    return checker;
}

bool checkType_Call(SymbolTable *table, Call *call, vector<Procedure *> *procedures) {
    if (procedures == NULL) return false; // check if the pointer is null to avoid the null pointer exception

    int size = procedures->size();
    string targetName = *(call->name->i);
    Procedure *p = NULL;

    // use for loop to iterate the procedures
    for (int i = 0; i < size; i++) {
        Procedure *procedure = procedures->at(i);

        if (procedure->name->compare(targetName) == 0) {
            p = procedure;
            break;
        }
    }

    if (p == NULL) return false; // check if the pointer is null to avoid the null pointer exception

    int totalNumOfParams = call->params->size();
    bool checker = true;

    // use for loop to iterate parameters
    for (int i = 0; i < totalNumOfParams; i++) {
        Expression *paramExpression = call->params->at(i);
        char type = getType(table, paramExpression);

        // check if the type of the parameter that is used for the function call is valid
        if (!(p->params->at(i)->varType == type) && type != 0) {
            return false;
        }
    }

    return true;
}

bool checkType(SymbolTable *table, Stmt *stmt, vector<Procedure *> *procedures) {
    if (stmt == NULL) return false;

    bool checker = true;
    std::string *varName = NULL;

    // use switch statement to check the type of the current statement
    switch (stmt->type) {
        case 'v':
            // check if this Var contains either Assign or Id
            if (stmt->statement->v->type != 'i') {
                checker = checker && checkType(table, stmt->statement->v->v->a->e);
            }
            break;
        case 'p':
            break;
        case 'g':
            break;
        case 'w':
            checker = checker && checkType(table, stmt->statement->w->e);
            checker = checker && checkType(table, stmt->statement->w->s1->stmts, procedures);
            break;
        case 'i':
            // check if this If union contains either If1 or If2
            if (stmt->type != '1') {
                If2 *if2 = stmt->statement->i->i->if2;
                checker = checker && checkType(table, if2->e);
                checker = checker && checkType(table, if2->s1->stmts, procedures);
                checker = checker && checkType(table, if2->s2->stmts, procedures);
            } else {
                If1 *if1 = stmt->statement->i->i->if1;
                checker = checker && checkType(table, if1->e);
                checker = checker && checkType(table, if1->s->stmts, procedures);
            }
            break;
        case 'a':
            checker = checker && checkType(table, stmt->statement->a->e);
            break;
        case 'c':
            checker = checker && checkType_Call(table, stmt->statement->c, procedures);
            break;
        default:
            std::cerr << "Error::Invalid statement type!" << std::endl;
            std::exit(EXIT_FAILURE);
    }

    return checker;
}

bool checkType(SymbolTable *table, vector<Stmt *> *stmts, vector<Procedure *> *procedures) {
    if (stmts == NULL) return false;

    int size = stmts->size();
    bool checker = true;

    // use for loop to iterate statements in the vector
    for (int i = 0; i < size; i++) {
        Stmt *stmt = stmts->at(i);
        checker = checker && checkType(table, stmt, procedures);
    }

    return checker;
}
