#include "ast.h"
#include <iostream>
#include <fstream>


int branchNum = 0;
const string BRANCH_SECTION_STR = "L_TEMP";

string *generateBranchSectionNum() {
    string name = BRANCH_SECTION_STR + std::to_string(branchNum);
    branchNum += 1;
    string *str = new string(name);
    return str;
}

/**
 * Generate HLA string for basic operations.
 * @param {a1} operand 1
 * @param {a2} operand 2
 * @param {a3} operand 3
 * @param {op} operator
 * 
 * i.e. a1 = a2 + a3
 * 
 * @return HLA string for basic operations.
 */
string *generateHLAString_Op(string *a1, string *a2, string *a3, string op) {
    string str = "    mov(";
    str += *a2;
    str += ", eax);\n    ";
    str += op;
    str += "(";
    str += *a3;
    str += ", eax);\n    mov(eax, ";
    str += *a1;
    str += ");\n";

    return new string(str);
}

/**
 * Generate HLA string for basic operations.
 * @param {a1} operand 1
 * @param {a2} operand 2
 * @param {a3} operand 3
 * 
 * i.e. a1 = a2 / a3
 * 
 * @return HLA string for division.
 */
string *generateHLAString_DivOp(string *a1, string *a2, string *a3) {
    string str = "    mov(0, edx)\n    mov(";
    str += *a2;
    str += ", eax);\n    mov(";
    str += *a3;
    str += ", ecx);\n    div(ecx);\n    mov(eax, ";
    str += *a1;
    str += ");\n";

    return new string(str);
}

string *generateHLA_CompareAndJump(string *a1, string *a2, string *a3, string op) {
    string str = "    mov(";
    str += *a2;
    str += ", eax);\n    mov(";
    str += *a3;
    str += ", ebx);\n    cmp(eax, ebx);\n    ";

    str += op; // > = jg,  < = jl, >= = jge, <= = jle, == = je, != = jne

    string *section1 = generateBranchSectionNum();
    string *section2 = generateBranchSectionNum();
    string *section3 = generateBranchSectionNum();

    str += (" " + *section1);
    str += ("\n    jmp " + *section2);
    str += "\n";
    str += *section1;
    str += ":\n    mov(1, ";
    str += *a1;
    str += ");\n    jmp ";
    str += *section3;
    str += "\n";
    str += (*section2 + ":\n    mov(0, ");
    str += *a1;
    str += ");\n    jmp ";
    str += *section3;
    str += "\n";
    str += *section3;
    str += ":\n";

    return new string(str);
}

/**
 * Generate HLA codes by parsing the TAC instance.
 * @param {tac} Three Address Code
 * @return HLA string
 */
string *generateHLAString(TAC *tac) {
    string str = "";

    string *a1 = tac->getA1();
    string *a2 = tac->getA2();
    string *a3 = tac->getA3();
    string *op = tac->getOp();

    // check if a1 is NULL
    if (a1 == NULL) return NULL;

    // check if this three address code instance contains the GOTO
    if (tac->hasGoto) {
        str += "    ";

        // check if this three address code is for "If t Goto L??" or "Goto L??"
        if (a1->compare("If") == 0) {
            str += "mov(";
            str += "0 , eax);\n    cmp(eax, ";
            str += *a2;
            str += ");\n    jne ";
            str += *a3;
            str += ";\n";

            return new string(str);
        }

        str += "jmp ";
        str += *(a1);
        str += ";\n";

        return new string(str);
    }

    // check if this is the starting point of the new section
    if (startsWith(*(a1), "L")) {
        return new string(*(a1) + ":\n");
    }

    // check if the TAC has an operator.
    if (op != NULL) {
        if (op->compare(">") == 0) {
            return generateHLA_CompareAndJump(a1, a2, a3, "jg"); //generate HLA for '>'

        } else if (op->compare(">=") == 0) {
            return generateHLA_CompareAndJump(a1, a2, a3, "jge"); //generate HLA for '>='

        } else if (op->compare("<") == 0) {
            return generateHLA_CompareAndJump(a1, a2, a3, "jl"); //generate HLA for '<'

        } else if (op->compare("<=") == 0) {
            return generateHLA_CompareAndJump(a1, a2, a3, "jle"); //generate HLA for '<='

        } else if (op->compare("!=") == 0) {
            return generateHLA_CompareAndJump(a1, a2, a3, "jne"); //generate HLA for '!='

        } else if (op->compare("==") == 0) {
            return generateHLA_CompareAndJump(a1, a2, a3, "je"); //generate HLA for '=='

        } else if (op->compare("+") == 0) {
            return generateHLAString_Op(a1, a2, a3, "add"); //generate HLA for '+'

        } else if (op->compare("-") == 0) {
            return generateHLAString_Op(a1, a2, a3, "sub"); //generate HLA for '-'

        } else if (op->compare("*") == 0) {
            return generateHLAString_Op(a1, a2, a3, "intmul"); //generate HLA for '*'

        } else if (op->compare("/") == 0) {
            return generateHLAString_DivOp(a1, a2, a3); //generate HLA for '/'

        } else if (op->compare("&&") == 0) {
            return generateHLAString_Op(a1, a2, a3, "and"); //generate HLA for '&&'

        } else if (op->compare("||") == 0) {
            return generateHLAString_Op(a1, a2, a3, "or"); //generate HLA for '||'

        } else {
            return NULL; //return NULL for unexpected operator
        }
    }

    if (tac->hasAssign) { //check if the TAC contains the assignment
        str += "    mov(";
        str += *a2;
        str += ", ";
        str += *a1;
        str += ");\n";
    } else {
        // check if this tac is for function call
        if (a1->compare("Call") == 0) {
            str += "    ";
            str += (*a2 + "();\n");
        } else {
            return NULL;
        }
    }

    return new string(str);
}

void writeHLA_Call(string *functionName, vector<string> &params, ofstream &hlaFile) {
    hlaFile << "    ";
    hlaFile << *functionName;
    hlaFile << "(";

    int size = params.size();
    int lastIndex = size - 1;

    for (int i = 0; i < size; i++) {
        hlaFile << params.at(i);

        if (i != lastIndex) {
            hlaFile << ", ";
        }
    }

    hlaFile << ");\n";
}

void writeHLA(TACList *tacList, ofstream &hlaFile) {
    vector<TAC *> *list = tacList->getList();
    int size = list->size();

    // use for loop to iterate TAC instances in the list
    for (int i = 0; i < size; i++) {
        TAC *tac = list->at(i);

        // check if the current tac makes a function call with standard print function
        if (startsWith(*(tac->getA1()), "_Print")) {
            hlaFile << "    stdout.put(" << *(tac->getA2()) << " , nl);\n";
            continue;
        }

        // check if current tac is for "PushParam", which pushes the parameter to the stack for the function call
        if (tac->getA1()->compare("PushParam") == 0) {
            vector<string> params;

            params.push_back(*(tac->getA2())); //add param string to vector
            i += 1; //increase the value of i to parse the next tac instance

            // use for loop to iterate tac instances
            for (; i < size; i++) {
                tac = list->at(i);
                string *a1 = tac->getA1();

                // check if the current TAC is for either function call or PushParam
                if (a1->compare("Call") == 0) {
                    writeHLA_Call(tac->getA2(), params, hlaFile);
                    break;
                } else if (a1->compare("PushParam") == 0) {
                    params.push_back(*(tac->getA2()));
                } else {
                    break;
                }
            }

            continue;
        }

        string *str = generateHLAString(tac); //generate HLA code

        // check if the returned string is null
        if (str != NULL) {
            hlaFile << *str;
            delete str;
        }
    }
}

/**
 * Print out the parameters of the procedure.
 * @param {params} list of parameters
 * @param {size} the size of the list
 * @param {hlaFile} the file output stream
 */
void writeParamsOfProcedure(vector<Variable *> *params, int size, ofstream &hlaFile) {
    int lastIndex = size - 1;

    for (int i = 0; i < size; i++) {
        Variable *var = params->at(i);
        string str = "";

        if (var->type != 'i') {
            str += *(var->v->a->i->i);
        } else {
            str += *(var->v->id->i);
        }

        str += " : ";

        if (var->varType == 'i') {
            str += "int32";
        } else if (var->varType == 'b') {
            str += "boolean";
        } else {
            str += "string";
        }

        // check if the current index is the final index - if so, add semi colon to the string.
        if (i != lastIndex ) {
            str += "; ";
        } else {
            str += " ";
        }

        hlaFile << str;
    }
}

void generateVariableSection(SymbolTable *table, ofstream &hlaFile) {
    vector<VarInfo *> *list = table->getList();
    int size = list->size();

    if (size == 0) return;

    hlaFile << "var\n";

    // use for loop to iterate VarInfo instances in the symbol table
    for (int i = 0; i < size; i++) {
        VarInfo *info = list->at(i);

        string *name = info->name;
        char type = info->type;

        hlaFile << "    " << *name << ":";

        if (type == 'i') {
            hlaFile << "int32\n";
        } else if (type == 'b') {
            hlaFile << "boolean\n";
        } else {
            hlaFile << "string\n";
        }
    }
}

/**
 * Generates the HLA file by parsing the TACs.
 * @param {list} the list of TAC instances
 * @param {procedures} the list of procedures
 * @param {programName} the name of the program
 * @param {table} the symbol table
 */
void generateHLA(vector<TACList *> list, vector<Procedure *> *procedures, string *programName, SymbolTable *table) {
    int size = list.size();

    // check if the list is empty
    if (size == 0) {
        cout << "Error::No three address code to parse!" << endl;
        return;
    }

    // validate the size of lists
    if (size - 1 != procedures->size()) {
        cout << "Error::Invalid size of procedures!" << endl;
        cout << "     expected = " << (size - 1) << endl;
        cout << "     actual = " << procedures->size() << endl;
        return;
    }

    ofstream hlaFile;
    hlaFile.open("hla.txt");

    // write program name and include statement to the HLA file
    hlaFile << "program " << *(programName) << ";\nbegin " << *(programName) << ";\n\n#include( \"stdlib.hhf\" );\n\n";

    // iterate instances of the list of three address code
    for (int i = 1; i < size; i++) {
        TACList *tacList = list.at(i);
        Procedure *p = procedures->at(i - 1);
        string *name = p->name;

        /* write HLA codes via file output stream */

        hlaFile << "procedure " << *(name);

        // get the list of parameters of the procedure
        vector<Variable *> *params = p->params;
        int size = params->size();

        // check if the procedure has parameters
        if (size != 0) {
            hlaFile << " ( ";
            writeParamsOfProcedure(params, size, hlaFile); // write HLA code for parameters of procedure
            hlaFile << ")";
        }

        hlaFile << ";  @noframe\n";

        hlaFile << "begin " << *(name) << ";\n\n";

        writeHLA(tacList, hlaFile); // generate HLA for procedures

        hlaFile << "\nend " << *(name) << ";\n";
    }

    // start writing HLA codes for main function
    hlaFile << "procedure main;  @noframe\n";

    generateVariableSection(table, hlaFile); //generate HLA codes for var section

    hlaFile << "begin main;\n";

    TACList *mainList = list.at(0); //get the TACList for main function

    writeHLA(mainList, hlaFile); // generate HLA for main function

    hlaFile << "\nend main;\n\nend " << *(programName) << ";\n";

    hlaFile.close(); //close the file stream
}
