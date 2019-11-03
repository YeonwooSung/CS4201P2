#include "ast.h"
#include <iostream>

TACList *list;

void generateTAC_Var(Variable *var) {
    TAC *tac = new TAC();

    if (var->type != 'i') {
        //TODO Assign
    }

    list->appendTAC(tac);
}

void generateTAC_Print(Print *p) {
    TAC *tac = new TAC();

    //

    list->appendTAC(tac);
}

void generateTAC(vector<Stmt *> *stmts) {
    int size = stmts->size();

    for (int i = 0; i < size; i++) {
        Stmt *stmt = stmts->at(i);

        // use switch statement to check the type of the current statement
        switch (stmt->type) {
            case 'v':
                generateTAC_Var(stmt->statement->v);
                break;
            case 'p':
                //TODO
                break;
            case 'g':
                //TODO
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
                //TODO
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

TACList *generateTACList(vector<Stmt *> *stmts) {
    list = new TACList();
    generateTAC(stmts);
    return list;
}
