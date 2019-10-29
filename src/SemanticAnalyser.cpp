#include "ast.h"

#include <iostream>

int main() {
    SymbolTable *table = new SymbolTable();
    Program *program = getExampleTree1(table);

    std::cout << "size: " << table->getSizeOfList() << "\n";
}
