#include "ast.h"
#include <iostream>
#include <fstream>


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
            str += ");\n    jne( ";
            str += *a3;
            str += " );\n";

            return new string(str);
        }

        str += "jmp(";
        str += *(a1);
        str += ");\n";

        return new string(str);
    }

    // check if this is the starting point of the new section
    if (startsWith(*(a1), "L")) {
        return new string(*(a1) + ":\n");
    }

    // check if the TAC has an operator.
    if (op != NULL) {
        if (op->compare(">") == 0) {
            //TODO
        } else if (op->compare(">=") == 0) {
            //TODO
        } else if (op->compare("<") == 0) {
            //TODO
        } else if (op->compare("<=") == 0) {
            //TODO
        } else if (op->compare("!=") == 0) {
            //TODO
        } else if (op->compare("==") == 0) {
            //TODO
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
        //TODO
    }

    return new string(str);
}

void writeHLA(TACList *tacList, ofstream &hlaFile) {
    vector<TAC *> *list = tacList->getList();
    int size = list->size();

    // use for loop to iterate TAC instances in the list
    for (int i = 0; i < size; i++) {
        TAC *tac = list->at(i);

        // check if the current tac makes a function call with standard print function
        if (startsWith(*(tac->getA1()), "_Print")) {
            if (tac->getA1()->compare("_PrintInt") == 0) {
                //TODO
            } else if (tac->getA1()->compare("_PrintString") == 0) {
                //TODO
            } else if (tac->getA1()->compare("_PrintBool") == 0) {
                //TODO
            }

            continue;
        }

        string *str = generateHLAString(tac);

        // check if the returned string is null
        if (str != NULL) {
            hlaFile << *str;
            delete str;
        }
    }
}

void generateHLA(vector<TACList *> list, vector<Procedure *> *procedures, string *programName) {
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
        string *name = procedures->at(i - 1)->name;

        /* write HLA codes via file output stream */

        hlaFile << "procedure " << *(name) << ";  @noframe\n";
        hlaFile << "begin " << *(name) << ";\n\n";

        writeHLA(tacList, hlaFile); // generate HLA for procedures

        hlaFile << "\nend " << *(name) << ";\n";
    }

    // start writing HLA codes for main function
    hlaFile << "procedure main;  @noframe\n";
    hlaFile << "begin main;\n";

    TACList *mainList = list.at(0); //get the TACList for main function

    writeHLA(mainList, hlaFile); // generate HLA for main function

    hlaFile << "\nend main;\n\nend " << *(programName) << ";\n";

    hlaFile.close(); //close the file stream
}
