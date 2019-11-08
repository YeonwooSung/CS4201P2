#include "ast.h"
#include <iostream>


int varNum;
int sectionNum = 0;
const string VARNAME = "@TEMP_";
const string SECTION_STR = "L";

void generateTAC(SymbolTable *table, vector<Stmt *> *stmts, TACList *list);
bool processExpression(SymbolTable *table, Expression *expr, TACList *list, TAC *tac);


string *generateSectionNum() {
    string name = SECTION_STR + std::to_string(sectionNum);
    sectionNum += 1;
    string *str = new string(name);
    return str;
}

/**
 * Generate the variable name for Three Address Code.
 * @return The generated name
 */
string *generateVarName() {
    string name = VARNAME + std::to_string(varNum);
    varNum += 1;
    string *varName = new string(name);
    return varName;
}

/**
 * Generates the suitable string for given Factor.
 * @param {table} symbol table
 * @param {f} Factor
 * @param {list} list of TAC
 * @return The generated factor string.
 */
string *generateFactorString(SymbolTable *table, Factor *f, TACList *list) {
    if (f->con != NULL) {
        return new string(*(f->con));

    } else if (f->e != NULL) {
        TAC *tac = new TAC();
        tac->setA1(generateVarName()); //set first address of TAC with temp name
        tac->hasAssign = true;

        if (processExpression(table, f->e, list, tac)) {
            delete tac;
            return NULL;
        }
        list->appendTAC(tac);

        return new string(*(tac->getA1()));

    } else if (f->id != NULL) {
        return new string(*(f->id->i));

    } else if (f->note != NULL) {
        TAC *tac = new TAC();
        tac->hasAssign = true;
        tac->setA1(generateVarName()); //set first address of TAC with temp name

        if (processExpression(table, f->note, list, tac)) {
            delete tac;
            return NULL;
        }

        list->appendTAC(tac);
        return new string(*(tac->getA1()));
    }

    return NULL;
}

string *generateTAC_Term(SymbolTable *table, Term *t, TACList *list) {
    TAC *tac = new TAC();
    tac->setA1(generateVarName());
    tac->hasAssign = true;

    // Check whether Term1 is used for Term. Otherwise, Term2 is used.
    if (t->t1 != NULL) {
        Factor *f = t->t1->f;
        string *s = generateFactorString(table, f, list);

        if (s != NULL) {
            tac->setA2(s);
        } else {
            return NULL;
        }
    } else {
        Term2 *t2 = t->t2;
        MulOp mulop = *(t2->op);

        // use switch statement to check which operator is used
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
                return NULL;
        }

        string *s = generateFactorString(table, t2->f1, list); //get the factor string

        // check if returned string is NULL
        if (s != NULL) {
            tac->setA2(s);
        } else {
            return NULL;
        }

        s = generateFactorString(table, t2->f2, list); //get the factor string

        // check if returned string is NULL
        if (s != NULL) {
            tac->setA3(s);
        } else {
            return NULL;
        }
    }

    list->appendTAC(tac); //append the generated ThreeAddressCode instance to the list

    return tac->getA1();
}

bool processSimpleExpression(SymbolTable *table, Simple *s, TAC *tac, TACList *list) {
    if (s->e1 != NULL) {
        Term *t = s->e1->t;

        if (t->t1 != NULL) {
            Factor *f = t->t1->f;
            string *s = generateFactorString(table, f, list);

            if (s != NULL) {
                tac->setA2(s);
            } else {
                return true;
            }

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

            string *s = generateFactorString(table, t2->f1, list);
            // check if returned string is NULL
            if (s != NULL) {
                tac->setA2(s);
            } else {
                return true;
            }

            s = generateFactorString(table, t2->f2, list);
            // check if returned string is NULL
            if (s != NULL) {
                tac->setA3(s);
            } else {
                return true;
            }

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

        tac->setA2(generateTAC_Term(table, s2->t1, list));
        tac->setA3(generateTAC_Term(table, s2->t2, list));

    } else {
        return true;
    }

    return false;
}

bool processExpression(SymbolTable *table, Expression *expr, TACList *list, TAC *tac) {
    // check the type of expression
    if (expr->type != 's') {
        Expr2 *e = expr->expr->e2;
        Simple *s1 = e->e1;
        Simple *s2 = e->e2;

        TAC *tac1 = new TAC();
        tac1->hasAssign = true;

        // Check if the Simple is valid. If not, free the assigned memory.
        if (processSimpleExpression(table, s1, tac1, list)) {
            delete tac1;
            return true;
        }

        tac1->setA1(generateVarName()); //set the first address with the temp name

        TAC *tac2 = new TAC();
        tac2->hasAssign = true;

        // Check if the Simple is valid. If not, free the assigned memory.
        if (processSimpleExpression(table, s2, tac2, list)) {
            delete tac1;
            delete tac2;
            return true;
        }

        tac2->setA1(generateVarName()); //set the first address with the temp name

        // append the TAC instaces to the list
        list->appendTAC(tac1);
        list->appendTAC(tac2);

        string *str1 = new string(*(tac1->getA1()));
        string *str2 = new string(*(tac2->getA1()));
        RelOp relop = *(e->op);

        tac->setA2(str1);
        tac->setA3(str2);

        switch(relop) {
            case Eq:
                tac->setOp(new string("=="));
                break;
            case NEq:
                tac->setOp(new string("!="));
                break;
            case LT:
                tac->setOp(new string("<"));
                break;
            case LTE:
                tac->setOp(new string("<="));
                break;
            case GT:
                tac->setOp(new string(">"));
                break;
            case GTE:
                tac->setOp(new string(">="));
                break;
            default:
                delete tac1;
                delete tac2;
                return true;
        }

    } else {
        Simple *s = expr->expr->e1;

        if (processSimpleExpression(table, s, tac, list)) {
            return true;
        }
    }

    return false;
}

void generateTAC_Assign(SymbolTable *table, Assign *a, TACList *list) {
    TAC *tac = new TAC();

    tac->hasAssign = true;
    tac->setA1(new string(*(a->i->i))); // set the first address with the name of the variable

    // check if the expression is valid
    if (processExpression(table, a->e, list, tac)) {
        delete tac;
        return;
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
 * Generate Three Address Code for function call.
 * @param {table} symbol table
 * @param {c} Function call
 * @param {list} TACList instance
 */
void generateTAC_Call(SymbolTable * table, Call *c, TACList* list) {
    vector<Expression *> *params = c->params;
    int size = params->size();
    vector<TAC *> tempList;

    // use for loop to iterate the parameters of the function call
    for (int i = 0; i < size; i++) {
        Expression *e = params->at(i);

        TAC *param = new TAC();

        // check if the expression is valid
        if (processExpression(table, e, list, param)) {
            delete param;
        } else {
            param->setA1(generateVarName());
            param->hasAssign = true;

            TAC *tac = new TAC();
            tac->setA1(new string("PushParam"));
            tac->setA2(new string(*(param->getA1())));

            list->appendTAC(param);
            tempList.push_back(tac);
        }
    }

    size = tempList.size();

    // use for loop to iterate the elements in the tempList
    for (int i = 0; i < size; i++) {
        TAC *tac = tempList.at(i);
        list->appendTAC(tac); //append the TAC instance to the list
    }

    // TAC for Call command
    TAC *tac = new TAC();
    tac->setA1(new string("Call"));
    tac->setA2(new string(*(c->name->i)));
    list->appendTAC(tac);

    // TAC for "PopParams"
    TAC *popParams = new TAC();
    popParams->setA1(new string("PopParams"));
    list->appendTAC(popParams);
}

void generateTAC_If(SymbolTable *table, If *i, TACList *list) {
    TAC *expression = new TAC();
    expression->setA1(generateVarName());
    expression->hasAssign = true;

    TAC *tac = new TAC();
    tac->setA1(new string("If"));
    tac->setA2(new string(*(expression->getA1())));
    tac->hasGoto = true;

    // check if the IfStmt union contains either If1 or If2
    if (i->type != '1') {
        If2 *if2 = i->i->if2;
        if (processExpression(table, if2->e, list, expression)) {
            delete expression;
            delete tac;
            varNum -= 1;
            return;
        } else {
            list->appendTAC(expression);

            // TAC object for "If t Goto L0"
            string *str1 = generateSectionNum();
            tac->setA3(new string(*(str1)));
            list->appendTAC(tac);

            // TAC object for "Goto L1"
            tac = new TAC();
            string *str2 = generateSectionNum();
            tac->setA1(new string(*(str2)));
            tac->hasGoto = true;
            list->appendTAC(tac);

            // TAC object for "L0:"
            tac = new TAC();
            tac->setA1(str1);
            tac->isStartOfSection = true;
            list->appendTAC(tac);

            generateTAC(table, if2->s1->stmts, list); //process statements in the If statement

            // TAC object for "L1:"
            tac = new TAC();
            tac->setA1(str2);
            tac->isStartOfSection = true;
            list->appendTAC(tac);

            generateTAC(table, if2->s2->stmts, list); //process statements in the else statement
        }
    } else {
        If1 *if1 = i->i->if1;
        if (processExpression(table, if1->e, list, expression)) {
            delete expression;
            delete tac;
            varNum -= 1;
            return;
        } else {
            list->appendTAC(expression);

            // TAC object for "If t Goto L0"
            string *str1 = generateSectionNum();
            tac->setA3(new string(*(str1)));
            list->appendTAC(tac);

            // TAC object for "Goto L1"
            tac = new TAC();
            string *str2 = generateSectionNum();
            tac->setA1(new string(*(str2)));
            tac->hasGoto = true;
            list->appendTAC(tac);

            // TAC object for "L0:"
            tac = new TAC();
            tac->setA1(str1);
            tac->isStartOfSection = true;
            list->appendTAC(tac);

            generateTAC(table, if1->s->stmts, list); //process statements in the If statement

            // TAC object for "L1:"
            tac = new TAC();
            tac->setA1(str2);
            tac->isStartOfSection = true;
            list->appendTAC(tac);
        }
    }
}

/**
 * Generate Three Address Code for While statement.
 * @param {table} symbol table
 * @param {w} While statement
 * @param {list} The list of TAC instances
 */
void generateTAC_While(SymbolTable *table, While *w, TACList *list) {
    TAC *expression = new TAC();
    expression->setA1(generateVarName());
    expression->hasAssign = true;

    // check if the boolean expression of the while statement is valid
    if (processExpression(table, w->e, list, expression)) {
        delete expression;
        return;
    }

    expression->setA1(generateVarName());
    list->appendTAC(expression);

    // TAC object for "If t Goto L0"
    TAC *tac_if = new TAC();
    tac_if->setA1(new string("If"));
    tac_if->setA2(new string(*(expression->getA1())));
    tac_if->hasGoto = true;
    string *str1 = generateSectionNum();
    tac_if->setA3(new string(*(str1)));
    list->appendTAC(tac_if);

    // TAC object for "Goto L1"
    TAC *tac_goto = new TAC();
    string *str2 = generateSectionNum();
    tac_goto->setA1(new string(*(str2)));
    tac_goto->hasGoto = true;
    list->appendTAC(tac_goto);

    // TAC object for "L0:"
    TAC *label1 = new TAC();
    label1->setA1(str1);
    label1->isStartOfSection = true;
    list->appendTAC(label1);

    generateTAC(table, w->s1->stmts, list); //process statements in the While statement

    // TAC object for "If t Goto L0"
    tac_if = new TAC();
    tac_if->setA1(new string("If"));
    tac_if->setA2(new string(*(expression->getA1())));
    tac_if->hasGoto = true;
    tac_if->setA3(new string(*(str1)));
    list->appendTAC(tac_if);

    // TAC object for "Goto L1"
    tac_goto = new TAC();
    tac_goto->setA1(new string(*(str2)));
    tac_goto->hasGoto = true;
    list->appendTAC(tac_goto);

    // TAC object for "L1:"
    TAC *label2 = new TAC();
    label2->setA1(str2);
    label2->isStartOfSection = true;
    list->appendTAC(label2);
}

/**
 * Generate the three address code for the statements.
 * @param {table} symbol table
 * @param {stmts} list of Stmt instances
 * @param {list} list of Three Address Code
 */
void generateTAC(SymbolTable *table, vector<Stmt *> *stmts, TACList *list) {
    int size = stmts->size();

    // use for loop to iterate statements
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
                generateTAC_While(table, stmt->statement->w, list);
                break;
            case 'i':
                generateTAC_If(table, stmt->statement->i, list);
                break;
            case 'a':
                generateTAC_Assign(table, stmt->statement->a, list);
                break;
            case 'c':
                generateTAC_Call(table, stmt->statement->c, list);
                break;
            default:
                std::cerr << "Error::Invalid statement type!" << std::endl;
                std::exit(EXIT_FAILURE);
        }
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
