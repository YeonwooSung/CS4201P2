#include "ast.h"
#include <iostream>
#include <fstream>

/**
 * Checks if the given string starts with the specific string.
 *
 * @param {s1} target string
 * @param {s2} shorter string
 * @return If the s1 starts with s2, returns true. Otherwise, returns false.
 */
bool startsWith(const string& s1, const string& s2) {
    return s2.length() <= s1.length() && equal(s2.begin(), s2.end(), s1.begin());
}

string *generateTACString(TAC *tac) {
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
            str += "If ";
            str += *(a2);
            str += " Goto ";
            str += *(a3);
            str += ";";

            return new string(str);
        }

        str += ("Goto " + *(a1));
        str += ";";
        return new string(str);
    }

    // check if this is the starting point of the new section
    if (startsWith(*(a1), "L")) {
        return new string(*(a1) + ":");
    }

    /* check if pointer is null, and build the "three address code" string */

    str += "    ";
    str += *(a1);

    if (tac->hasAssign) str += " =";

    if (a2 != NULL) str += (" " + *(a2));
    if (op != NULL) str += (" " + *(op));
    if (a3 != NULL) str += (" " + *(a3));

    str += ";";

    return new string(str);
}

void writeTAC(TACList *tacList, ofstream &tacFile, string functionName) {
    vector<TAC *> *list = tacList->getList();
    int size = list->size();

    tacFile << functionName << ":\n";
    tacFile << "    BeginFunc;" << "\n";

    // use for loop to iterate TAC instances in the list
    for (int i = 0; i < size; i++) {
        TAC *tac = list->at(i);

        // check if the current tac makes a function call with standard print function
        if (startsWith(*(tac->getA1()), "_Print")) {
            tacFile << "    PushParam " << *(tac->getA2()) << ";\n";
            tacFile << "    Call " << *(tac->getA1()) << ";\n";
            tacFile << "    PopParams;\n";

            continue;
        }

        string *str = generateTACString(tac);

        // check if the returned string is null
        if (str != NULL) {
            tacFile << *(str) << "\n";
            delete str;
        }
    }
}

void writeTAC(vector<TACList *> list, vector<Procedure *> *procedures) {
    int size = list.size();

    // check if the list is empty
    if (size == 0) {
        cout << "Error::No three address code!" << endl;
        return;
    }

    // validate the size of lists
    if (size - 1 != procedures->size()) {
        cout << "Error::Invalid size of procedures!" << endl;
        cout << "     expected = " << (size - 1) << endl;
        cout << "     actual = " << procedures->size() << endl;
        return;
    }

    ofstream tacFile;
    tacFile.open("tac.txt");

    // iterate instances of the list of three address code
    for (int i = 1; i < size; i++) {
        TACList *tacList = list.at(i);
        Procedure *p = procedures->at(i - 1);
        writeTAC(tacList, tacFile, *(p->name)); // call method to write three address codes for the procedure
    }

    TACList *mainList = list.at(0);
    writeTAC(mainList, tacFile, "main"); // call method to write three address codes for the main function

    tacFile.close();
}
