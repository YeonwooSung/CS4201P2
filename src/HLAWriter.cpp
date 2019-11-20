#include "ast.h"
#include <iostream>
#include <fstream>


int branchNum = 0;
const string BRANCH_SECTION_STR = "L_TEMP";

/**
 * This function generates the section label for the branching.
 * When the HLA jumps for the branching, it requires the label name of the target section.
 * And this is the function that generates that label name for the branching.
 * @return The generated label.
 */
string *generateBranchSectionNum() {
    string name = BRANCH_SECTION_STR + std::to_string(branchNum); //generate label name
    branchNum += 1; //increase the label number
    string *str = new string(name); //allocate memory
    return str;
}

/**
 * Generate HLA string for basic operations.
 * @param {a1} operand 1
 * @param {a2} operand 2
 * @param {a3} operand 3
 * @param {op} operator
 * 
 * a1 = a2 op a3
 *
 * i.e. a1 = a2 + a3
 * i.e. a1 = a2 || a3
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
 * This function generates HLA codes for branching.
 * For the conditional operators (>, >=, <, <=, ==, !=) we could either use conditional
 * instructions (i.e. setle, setge, etc) or branching.
 * It might be easier to use conditional instructions for the conditional operators.
 * However, I decided to use branching to implement the conditional operators, since this is more
 * challenging one.
 */
string *generateHLA_CompareAndJump(string *a1, string *a2, string *a3, string op) {
    string str = "    mov(";
    str += *a2;
    str += ", eax);\n    mov(";
    str += *a3;
    str += ", ebx);\n    cmp(eax, ebx);\n    ";

    str += op; // jg for >, jl for <, jge for >=, jle for <=, je for ==, jne for !=

    /*
     * The conditional branching method that I implemented requires 3 labels.
     * I used the three address code to check if the given conditional operation is true.
     * So, by using the branching with 3 labels, we could set the suitable boolean value to the
     * first variable of the three address code.
     *
     * i.e. a = b > c
     *
     * cmp b c
     * jg L0
     * jmp L1
     * 
     * L0: mov true a
     * jmp L2
     * 
     * L1: mov false a
     * jmp L2
     * 
     * L2:
     */

    string *section1 = generateBranchSectionNum(); //generate the label for L0
    string *section2 = generateBranchSectionNum(); //generate the label for L1
    string *section3 = generateBranchSectionNum(); //generate the label for L2

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

        /* Use the if-else statement to find the operator, and execute the corresponding function */

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

    if (a1->compare("Return") == 0) { // check if the TAC contains the return statement
        /*
         * The HLA provides 2 different ways for return statement.
         * 1) use ret() function
         * 2) use @returns("text") annotation to specify the register that contains the return value.
         *
         * To implement the return statement, I decided to use @returns annotation with eax.
         * So, my program will add a "@returns("eax")" annotation to the procedure if the procedure has
         * the return statement.
         * Then, in the procedure body, the program will add a line "mov(return_value, eax)" instruction
         * to assign the value that should be returned to the "eax" register.
         */
        str += "    mov(";
        str += *a2;
        str += ", eax);\n";

    } else if (tac->hasAssign) { //check if the TAC contains the assignment
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

/**
 * This method writes the HLA codes via file output stream.
 * @param {functionName} the name of the function
 * @param {params} the list of parameters
 * @param {hlaFile} the instance for the file output stream
 */
void writeHLA_Call(string *functionName, vector<string> &params, ofstream &hlaFile) {
    hlaFile << "    ";
    hlaFile << *functionName;
    hlaFile << "(";

    int size = params.size();
    int lastIndex = size - 1;

    // use for loop to iterate parameters in the list
    for (int i = 0; i < size; i++) {
        hlaFile << params.at(i); //print out the parameter via file output stream

        if (i != lastIndex) {
            hlaFile << ", ";
        }
    }

    hlaFile << ");\n";
}

/**
 * The aim of this function is to write the HLA codes via file output stream.
 * @param {tacList} the instance that contains TAC instances
 * @param {hlaFile} the file output stream
 * @param {name} The name of the current procedure
 */
void writeHLA(TACList *tacList, ofstream &hlaFile, string &name) {
    vector<TAC *> *list = tacList->getList();
    int size = list->size();
    int lastIndex = size - 1;

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

            /*
             * Check if the current tac contains the return statement.
             * Then it will also check if this tac is not the last statement of the procedure.
             * Since the procedure meets the "return" statement, the procedure should end up there.
             * Thus, we need to add "exit procedure" instruction to the HLA file if the current return
             * statement is not located at the end of the procedure.
             */
            if (tac->getA1()->compare("Return") == 0 && i != lastIndex) {
                hlaFile << "    exit " << name << "\n";
            }
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

        // check if it is whether Assign type or Id type.
        if (var->type != 'i') {
            str += *(var->v->a->i->i);
        } else {
            str += *(var->v->id->i);
        }

        str += " : ";

        // check the type of the variable
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

/**
 * The aim of this function is to write the "var" section for the procedures.
 * According to the quick guide of the High Level Assembly, the procedure in the HLA
 * has a section called "var", which allows the programmers to declare the local variables.
 * Therefore, I made this function to generate "var" section for the procedures.
 *
 * The main reason that this function requires symbol table is to check the type of the variable,
 * so that this function could print out the suitable type name to the var section.
 *
 * @param {table} the symbol table
 * @param {hlaFile} file output stream
 */
void generateVariableSection(SymbolTable *table, ofstream &hlaFile) {
    vector<VarInfo *> *list = table->getList();
    int size = list->size();

    if (size == 0) return; //if there is no variables in the symbol table, end up the function

    /* write a HLA codes for the var section */

    hlaFile << "var\n";

    // use for loop to iterate VarInfo instances in the symbol table
    for (int i = 0; i < size; i++) {
        VarInfo *info = list->at(i);

        string *name = info->name;
        char type = info->type;

        hlaFile << "    " << *name << " : ";

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
 * Check if the current procedure has the return statement.
 * And if so, add the "@returns("eax")" annotation to the HLA code
 * @param {tacList} the list of TAC instances
 * @param {hlaFile} file output stream
 */
void checkAndGenerateReturnAnnotation(TACList *tacList, ofstream &hlaFile) {
    vector<TAC *> *list = tacList->getList();
    int lastIndex = list->size() - 1;

    // use for loop to iterate tac instances in the list
    for (int i = 0; i <= lastIndex; i++) {
        TAC *tac = list->at(i);
        if (tac->getA1()->compare("Return") == 0) {
            //add @returns("eax") annotation to let the HLA knows that this procedure returns some value.
            hlaFile << " @returns( \"eax\" );";
            break;
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
    hlaFile.open("hla.txt"); //open the file output stream

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

        //add @noframe annotation to prevent HLA inserting some additional codes that might make the program to fail to run as intended
        hlaFile << ";  @noframe;";

        checkAndGenerateReturnAnnotation(tacList, hlaFile); //add @returns annotation if required
        hlaFile << "\n";

        hlaFile << "begin " << *(name) << ";\n\n";
        writeHLA(tacList, hlaFile, *name); // generate HLA for procedures
        hlaFile << "\nend " << *(name) << ";\n";
    }

    hlaFile << "procedure main;  @noframe;"; // start writing HLA codes for main function

    TACList *mainList = list.at(0); //get the TACList for main function
    checkAndGenerateReturnAnnotation(mainList, hlaFile); //add @returns annotation if required
    hlaFile << "\n";

    generateVariableSection(table, hlaFile); //generate HLA codes for var section
    hlaFile << "begin main;\n";

    string name_mainFunction = "main";
    writeHLA(mainList, hlaFile, name_mainFunction); // generate HLA for main function
    hlaFile << "\nend main;\n\nend " << *(programName) << ";\n";

    hlaFile.close(); //close the file stream
}
