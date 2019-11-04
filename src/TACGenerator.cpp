#include "ast.h"
#include <iostream>


int varNum;
const string VARNAME = "@TEMP_";


string *generateVarName() {
    string name = VARNAME + std::to_string(varNum);
    varNum += 1;
    string *varName = new string(name);
    return varName;
}

string *generateFactorString(SymbolTable *table, Factor *f) {
    //TODO

    return NULL;
}

string *generateTAC_Term(SymbolTable *table, Term *var) {
    //TODO

    return NULL;
}

bool processSimpleExpression(SymbolTable *table, Simple *s, TAC *tac) {
    if (s->e1 != NULL) {
        Term *t = s->e1->t;

        if (t->t1 != NULL) {
            Factor *f = t->t1->f;
            tac->setA2(generateFactorString(table, f));

        } else if (t->t2 != NULL) {
            Term2 *t2 = t->t2;
            MulOp mulop = *(t2->op);

            switch(mulop) {
                case Mul:
                    tac->setOp(new string("*"));
                    break;
                case Div:
                    tac->setOp(new string("/"));
                    break;
                case And:
                    tac->setOp(new string("&&"));
                    break;
                default:
                    return true;
            }

            tac->setA2(generateFactorString(table, t2->f1));
            tac->setA3(generateFactorString(table, t2->f2));

        } else {
            return true;
        }

    } else if (s->e2 != NULL) {
        Simple2 *s2 = s->e2;
        AddOp addop = *(s2->op);

        switch(addop) {
            case Or:
                tac->setOp(new string("||"));
                break;
            case Plus:
                tac->setOp(new string("+"));
                break;
            case Minus:
                tac->setOp(new string("-"));
                break;
            default:
                return true;
        }

        tac->setA2(generateTAC_Term(table, s2->t1));
        tac->setA3(generateTAC_Term(table, s2->t2));

    } else {
        return true;
    }

    return false;
}

void generateTAC_Assign(SymbolTable *table, Assign *a, TACList *list) {
    TAC *tac = new TAC();

    tac->hasAssign = true;
    tac->setA1(new string(*(a->i->i))); // set the first address with the name of the variable

    // check the type of expression
    if (a->e->type != 's') {
        Expr2 *e = a->e->expr->e2;
        Simple *s1 = e->e1;
        Simple *s2 = e->e2;

        TAC *tac1 = new TAC();

        // Check if the Simple is valid. If not, free the assigned memory.
        if (processSimpleExpression(table, s1, tac1)) {
            delete tac1;
            delete tac;
            return;
        }

        tac1->setA1(generateVarName()); //set the first address with the temp name

        TAC *tac2 = new TAC();

        // Check if the Simple is valid. If not, free the assigned memory.
        if (processSimpleExpression(table, s2, tac2)) {
            delete tac1;
            delete tac2;
            delete tac;
            return;
        }

        tac2->setA1(generateVarName()); //set the first address with the temp name

        // append the TAC instaces to the list
        list->appendTAC(tac1);
        list->appendTAC(tac2);

        //TODO tac -> op (RelOp)
    } else {
        Simple *s = a->e->expr->e1;

        if (processSimpleExpression(table, s, tac)) {
            delete tac;
            return;
        }
    }

    list->appendTAC(tac);
}

/**
 * Generate the three address code for get statement.
 * @param {table} symbol table
 * @param {g} Get statement
 * @param {list} list of Three Address Code
 */
void generateTAC_Get(SymbolTable *table, Get *g, TACList *list) {
    string targetStr = *(g->id->i);
    char type = 0;

    vector<VarInfo *> *l = table->getList();
    int size = l->size();

    for (int i = 0; i < size; i++) {
        VarInfo *v = l->at(i);
        string *str = v->name;
        if (str->compare(targetStr) == 0) {
            type = v->type;
            break;
        }
    }

    if (type == 0) {
        type = 's';
    }

    TAC *tac = new TAC();
    tac->setA2(new string(targetStr));

    // check the type of variable.  'i' for int, 's' for string, 'b' for bool
    switch(type) {
        case 'b':
            tac->setA1(new string("_PrintBool"));
            break;
        case 's':
            tac->setA1(new string("_PrintString"));
            break;
        case 'i':
            tac->setA1(new string("_PrintInt"));
            break;
        default:
            tac->setA1(new string("_PrintString"));
    }

    list->appendTAC(tac);
}

/**
 * Generate the three address code for print statement.
 * @param {table} symbol table
 * @param {p} Print statement
 * @param {list} list of Three Address Code
 */
void generateTAC_Print(SymbolTable *table, Print *p, TACList *list) {
    TAC *tac = new TAC();
    tac->setA2(new string(*(p->s)));

    string targetStr = *(p->s);
    char type = 0;

    vector<VarInfo *> *l = table->getList();
    int size = l->size();

    for (int i = 0; i < size; i++) {
        VarInfo *v = l->at(i);
        string *str = v->name;
        if (str->compare(targetStr) == 0) {
            type = v->type;
            break;
        }
    }

    if (type == 0) {
        // check the type of the constant
        if (targetStr.compare("True") == 0 || targetStr.compare("False") == 0) {
            type = 'b'; //'b' for bool
        } else {
            string::const_iterator it = targetStr.begin();

            // to check if the value of the constant is the negative integer
            if (targetStr.at(0) == '-') {
                it += 1;
            }

            // use while loop to iterate the iterator
            while (it != targetStr.end() && std::isdigit(*it)) it += 1;

            if (!targetStr.empty() && it == targetStr.end()) {
                type = 'i'; //'i' for int
            } else {
                type = 's'; //'s' for string
            }
        }
    }

    // check the type.  'i' for int, 's' for string, 'b' for bool
    switch(type) {
        case 'b':
            tac->setA1(new string("_PrintBool"));
            break;
        case 's':
            tac->setA1(new string("_PrintString"));
            break;
        case 'i':
            tac->setA1(new string("_PrintInt"));
            break;
        default:
            tac->setA1(new string("_PrintString"));
    }

    list->appendTAC(tac);
}

/**
 * Generate the three address code for the statements.
 * @param {table} symbol table
 * @param {stmts} list of Stmt instances
 * @param {list} list of Three Address Code
 */
void generateTAC(SymbolTable *table, vector<Stmt *> *stmts, TACList *list) {
    int size = stmts->size();

    for (int i = 0; i < size; i++) {
        Stmt *stmt = stmts->at(i);

        // use switch statement to check the type of the current statement
        switch (stmt->type) {
            case 'v':
                if (stmt->statement->v->type != 'i') generateTAC_Assign(table, stmt->statement->v->v->a, list);
                break;
            case 'p':
                generateTAC_Print(table, stmt->statement->p, list);
                break;
            case 'g':
                generateTAC_Get(table, stmt->statement->g, list);
                break;
            case 'w':
                //TODO
                break;
            case 'i':
                // check if this If union contains either If1 or If2
                if (stmt->type != '1') {
                    If2 *if2 = stmt->statement->i->i->if2;
                    //TODO
                } else {
                    If1 *if1 = stmt->statement->i->i->if1;
                    //TODO
                }

                break;
            case 'a':
                generateTAC_Assign(table, stmt->statement->a, list);
                break;
            case 'c':
                //TODO
                break;
            default:
                std::cerr << "Error::Invalid statement type!" << std::endl;
                std::exit(EXIT_FAILURE);
        }

        //
    }
}

/**
 * Generate the list of TAC(Three Address Code) objects.
 * @params {stmts} - list of Stmt instances.
 * @return The pointer that points the generated list.
 */
TACList *generateTACList(SymbolTable *table, vector<Stmt *> *stmts) {
    TACList *list = new TACList();
    varNum = 0;
    generateTAC(table, stmts, list);
    return list;
}
