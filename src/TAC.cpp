#include "ast.h"

/* TAC */

TAC::TAC() {
    a1 = NULL;
    a2 = NULL;
    a3 = NULL;
    op = NULL;
    hasAssign = false;
}

TAC::~TAC() {
    /* Check if NULL, and free the allocated memory */
    if (a1 != NULL) delete a1;
    if (a2 != NULL) delete a2;
    if (a3 != NULL) delete a3;
    if (op != NULL) delete op;
}

void TAC::setA1(string *a1) {
    this->a1 = a1;
}

void TAC::setA2(string *a2) {
    this->a2 = a2;
}

void TAC::setA3(string *a3) {
    this->a3 = a3;
}

void TAC::setOp(string *op) {
    this->op = op;
}

string *TAC::getA1() {
    return this->a1;
}

string *TAC::getA2() {
    return this->a2;
}

string *TAC::getA3() {
    return this->a3;
}

string *TAC::getOp() {
    return this->op;
}


/* TACList */

TACList::TACList() {
    list = new vector<TAC *>;
}

TACList::~TACList() {
    // check if the pointer is NULL
    if (list != NULL) {
        //TODO delete elements in the list
        delete list;
    }
}

int TACList::appendTAC(TAC *tac) {
    list->push_back(tac);
    return list->size();
}
