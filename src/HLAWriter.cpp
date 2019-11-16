#include "ast.h"
#include <iostream>
#include <fstream>

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

        //TODO

        return new string(str);
    }

    // check if this is the starting point of the new section
    if (startsWith(*(a1), "L")) {
        return new string(*(a1) + ":");
    }

    //TODO

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

        if (str != NULL) {
            hlaFile << str << "\n";
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

        hlaFile << "procedure " << *(name) << ";  @noframe\n";
        hlaFile << "begin " << *(name) << ";\n\n";

        writeHLA(tacList, hlaFile); // generate HLA for procedures

        hlaFile << "\nend " << *(name) << ";\n";
    }

    hlaFile << "procedure main;  @noframe\n";
    hlaFile << "begin main;\n\n";

    TACList *mainList = list.at(0); //get the TACList for main function

    writeHLA(mainList, hlaFile); // generate HLA for main function

    hlaFile << "\nend main;\n\nend " << *(programName) << ";\n";

    hlaFile.close(); //close the file stream
}
