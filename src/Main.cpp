#include "ast.h"
#include <iostream>
#include <cstring>


int main(int argc, char **argv) {
    SymbolTable *table = new SymbolTable();
    Program *program = NULL;

    /*
     * If there is not command line arguments, use general tree.
     * If there is a command line argument, check the first argument.
     * If it is 1, use getExampleTree_error1() function.
     * Or if it is 2, use getExampleTree_error2() function.
     * Otherwise, just use the general tree.
     */
    if (argc > 1) {
        if (strcmp(argv[1], "1") == 0) {
            program = getExampleTree_error1(table); //get the AST that has semantic error in it (variable scoping issue)
        } else if (strcmp(argv[1], "2") == 0) {
            program = getExampleTree_error2(table); //get the AST that has type error (integer + boolean)
        } else {
            program = getExampleTree(table); //get the AST
        }
    } else {
        program = getExampleTree(table); //get the AST
    }

    vector<Procedure *> *procedures = program->ps; //get the list of procedures
    vector<Stmt *> *stmts = program->cs->stmts;    //get the list of statements in the compound of the main function

    // run the semantic analysis
    if (!checkScopeOfAllProcedures(table, procedures) || !checkScopeOfStmts(table, stmts, NULL, program->cs->startLine, program->cs->endLine)) {
        std::cerr << "Error::Semantic analysis failed" << std::endl;
        return 0;
    }

    // run the type checking
    if (!checkType(table, stmts, procedures)) {
        std::cerr << "Error::Type checking failed - Type error is found in the function \"main\"" << std::endl;
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
            std::cerr << "Error::Type checking failed - Type error found in the function \"" << *(p->name) << "\"" << std::endl;
            return 0;
        }

        table->remove(numOfParams); //remove the function parameters from the symbol table
    }

    vector<TACList *> list; //list that contains lists of ThreeAddressCode instances

    // generate three address codes for main method
    TACList *tacList_mainFunction = generateTACList(table, stmts);
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

    writeTAC(list, procedures); // write three address code file
    generateHLA(list, procedures, program->name, table); // generate HLA codes

    delete table;
    std::cout << "Program ends" << std::endl;
    return 1;
}
