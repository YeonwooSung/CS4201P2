#include "ast.h"
#include <iostream>


int main() {
    SymbolTable *table = new SymbolTable();
    Program *program = getExampleTree(table); //get the AST
    //Program *program = getExampleTree_error1(table); //get the AST that has semantic error in it (variable scoping issue)
    //Program *program = getExampleTree(table); //get the AST

    // run the semantic analysis
    if (!checkScopeOfAllProcedures(table, program->ps) || !checkScopeOfStmts(table, program->cs->stmts, NULL, program->cs->startLine, program->cs->endLine)) {
        std::cerr << "Error::Semantic analysis failed" << std::endl;
        return 0;
    }

    vector<Procedure *> *procedures = program->ps;

    // run the type checking
    if (!checkType(table, program->cs->stmts, procedures)) {
        std::cerr << "Error::Type checking failed - Main method" << std::endl;
        return 0;
    }

    int size = procedures->size();

    // use for loop to iterate procedures
    for (int i = 0; i < size; i++) {
        Procedure *p = procedures->at(i);

        vector<Variable *> *params = p->params;
        int numOfParams = params->size();

        // use for loop to check all parameters of the function
        for (int a = 0; a < numOfParams; a++) {
            Variable *v = params->at(a);

            // get information of the parameter

            VarInfo *info = new VarInfo;
            info->declaredLine = p->startLine;
            info->type = v->varType;

            // check the type of the union that is currently used, and get the name of the variable.

            if (v->type != 'v') {
                info->name = v->v->a->i->i;
            } else {
                info->name = v->v->id->i;
            }

            table->addVarInfo(info); //insert the function parameter to the symbol table
        }

        // run the type checker for each procedure
        if (!checkType(table, p->cs->stmts, procedures)) {
            std::cerr << "Error::Type checking failed - " << *(p->name) << std::endl;
            return 0;
        }

        table->remove(numOfParams); //remove the function parameters from the symbol table
    }

    vector<TACList *> list; //list that contains lists of ThreeAddressCode instances

    // generate three address codes for main method
    TACList *tacList_mainFunction = generateTACList(table, program->cs->stmts);
    TAC *endOfMainFunction = new TAC();
    endOfMainFunction->setA1(new string("EndFunc"));
    tacList_mainFunction->appendTAC(endOfMainFunction);
    list.push_back(tacList_mainFunction);

    // use for loop to iterate the procedures in the procedure list
    for (int i = 0; i < size; i++) {
        /* Generate three address codes for procedures */

        Procedure *p = procedures->at(i);

        TACList *tacList = generateTACList(table, p->cs->stmts);
        TAC *endOfFunction = new TAC();
        endOfFunction->setA1(new string("EndFunc"));

        tacList->appendTAC(endOfFunction);
        list.push_back(tacList);
    }

    // write three address code file
    writeTAC(list, procedures);

    // generate HLA codes
    generateHLA(list, procedures, program->name, table);

    delete table;
    std::cout << "Program ends" << std::endl;
    return 1;
}
