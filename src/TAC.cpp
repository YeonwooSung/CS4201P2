#include "ast.h"

/* TAC */

TAC::TAC() {
    a1 = NULL;
    a2 = NULL;
    a3 = NULL;
    op = NULL;
    hasAssign = false;
    hasGoto = false;
    isStartOfSection = false;
}

TAC::~TAC() {
    /* Check if NULL, and free the allocated memory */
    if (a1 != NULL) delete a1;
    if (a2 != NULL) delete a2;
    if (a3 != NULL) delete a3;
    if (op != NULL) delete op;
}

/**
 * Setter for a1.
 * @param {a1}
 */
void TAC::setA1(string *a1) {
    this->a1 = a1;
}

/**
 * Setter for a2.
 * @param {a2}
 */
void TAC::setA2(string *a2) {
    this->a2 = a2;
}

/**
 * Setter for a3.
 * @param {a3}
 */
void TAC::setA3(string *a3) {
    this->a3 = a3;
}

/**
 * Setter for op.
 * @param {op} operator
 */
void TAC::setOp(string *op) {
    this->op = op;
}

/**
 * Getter for a1.
 * @return a1
 */
string *TAC::getA1() {
    return this->a1;
}

/**
 * Getter for a2.
 * @return a2
 */
string *TAC::getA2() {
    return this->a2;
}

/**
 * Getter for a3.
 * @return a3
 */
string *TAC::getA3() {
    return this->a3;
}

/**
 * Getter for op.
 * @return op
 */
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

/**
 * Append TAC instance to the list.
 * @param {tac} Three Address Code
 * @return The size of the list.
 */
int TACList::appendTAC(TAC *tac) {
    list->push_back(tac);
    return list->size();
}

/**
 * Getter for list.
 * @return list
 */
vector<TAC *> *TACList::getList() {
    return this->list;
}
