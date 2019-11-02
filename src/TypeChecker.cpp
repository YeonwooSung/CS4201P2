#include "ast.h"
#include <iostream>

bool checkType(Expression *expression) {
    bool checker = true;

    //TODO

    return checker;
}

bool checkType(Stmt *stmt) {
    bool checker = true;
    std::string *varName = NULL;

    // use switch statement to check the type of the current statement
    switch (stmt->type) {
        case 'v':
            // check if this Var contains either Assign or Id
            if (stmt->statement->v->type != 'i') {
                checker = checker && checkType(stmt->statement->v->v->a->e);
            }
            break;
        case 'p':
            break;
        case 'g':
            break;
        case 'w':
            checker = checker && checkType(stmt->statement->w->e);
            checker = checker && checkType(stmt->statement->w->s1->stmts);
            break;
        case 'i':
            // check if this If union contains either If1 or If2
            if (stmt->type != '1') {
                If2 *if2 = stmt->statement->i->i->if2;
                checker = checker && checkType(if2->e);
                checker = checker && checkType(if2->s1->stmts);
                checker = checker && checkType(if2->s2->stmts);
            } else {
                If1 *if1 = stmt->statement->i->i->if1;
                checker = checker && checkType(if1->e);
                checker = checker && checkType(if1->s->stmts);
            }
            break;
        case 'a':
            checker = checker && checkType(stmt->statement->a->e);
            break;
        case 'c':
            //TODO
            break;
        default:
            std::cerr << "Error::Invalid statement type!" << std::endl;
            std::exit(EXIT_FAILURE);
    }

    return checker;
}

bool checkType(vector<Stmt *> *stmts) {
    int size = stmts->size();
    bool checker = false;

    for (int i = 0; i < size; i++) {
        Stmt *stmt = stmts->at(i);
        checkType(stmt);
    }

    return checker;
}
