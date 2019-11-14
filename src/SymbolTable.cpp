#include "ast.h"

/**
 * Constructor
 */
SymbolTable::SymbolTable() {
    list = new vector<VarInfo *>;
}

/**
 * Destructor
 */
SymbolTable::~SymbolTable() {
    if (list != NULL) {
        // remove instances in the vector
        int size = list->size();

        for (int i = 0; i < size; i++) {
            VarInfo *info = list->at(i);
            delete info;
        }

        delete list;
    }
}

/**
 * Getter for list.
 * @return list
 */
vector<VarInfo *> *SymbolTable::getList() {
    return list;
}

void SymbolTable::remove(int i) {
    int size = list->size();
    int limit = i;

    if (i > size) {
        limit = size;
    }

    for (int a = 0; a < limit; a++) {
        list->pop_back();
    }
}

VarInfo *SymbolTable::getVarInfo(string *name) {
    int size = list->size();
    VarInfo *res = NULL;

    for (int i = 0; i < size; i++) {
        VarInfo *info = list->at(i);
        string *nameOfInfo = info->name;

        if (name->compare(*nameOfInfo) == 0) {
            res = info;
            break;
        }
    }

    return res;
}

/**
 * Adds the VarInfo instace to the vector.
 * @param {v} new instance
 */
void SymbolTable::addVarInfo(VarInfo *v) {
    if (list == NULL) list = new vector<VarInfo *>;

    list->push_back(v);
}

/**
 * Gets the size of the list.
 * @return size of the list
 */
int SymbolTable::getSizeOfList() {
    return list->size();
}
