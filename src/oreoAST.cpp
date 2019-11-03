#include "ast.h"
#include <vector>

using namespace std;


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


Program *getExampleTree1(SymbolTable *table) {
    Program *p = new Program;
    p->ps = new vector<Procedure *>;

    Procedure *proc = new Procedure;
    string *procName = new string("printst");

    vector <Variable*>* procParams = new vector<Variable*>;
    Variable *procParameter = new Variable;
    Var *procParam = new Var;
    procParam->id = new Id;
    procParam->id->i = new string("str");
    procParameter->type = 'i';
    procParameter->v = procParam;
    procParams->push_back(procParameter);

    proc->name = procName;
    proc->params = procParams;
    proc->startLine = 5;
    proc->endLine = 17;

    Compound *procCompound = new Compound;
    proc->cs = procCompound;

    vector<Stmt *> *procStatements = new vector<Stmt *>;
    procCompound->stmts = procStatements;

    Statement *procStatement = new Statement;
    procStatement->p = new Print;
    procStatement->p->s = new string("str");

    Stmt *procStmt = new Stmt;
    procStmt->statement = procStatement;
    procStmt->type = 'p';

    procStatements->push_back(procStmt);

    p->ps->push_back(proc);


    // main function
    Compound *mainCompound = new Compound;
    mainCompound->stmts = new vector<Stmt *>;

    // variable declarations
    Statement *varNS = new Statement;
    Variable *variableN = new Variable;
    Var *varN = new Var;
    varN->id = new Id;
    varN->id->i = new string("n");

    Stmt *varNS_Stmt = new Stmt;
    varNS_Stmt->statement = varNS;
    varNS_Stmt->type = 'v';

    VarInfo *info1 = new VarInfo;
    info1->name = varN->id->i;
    info1->declaredLine = 22;
    table->addVarInfo(info1);

    variableN->v = varN;
    variableN->type = 'i';
    varNS->v = variableN;
    mainCompound->stmts->push_back(varNS_Stmt);

    Statement *varFirstS = new Statement;
    Var *varFirst = new Var;
    varFirst->id = new Id;
    varFirst->id->i = new string("first");
    
    Variable *variableFirst = new Variable;
    variableFirst->v = varFirst;
    variableFirst->type = 'i';

    Stmt *varFirstStmt = new Stmt;
    varFirstStmt->statement = varFirstS;
    varFirstStmt->type = 'v';

    VarInfo *info2 = new VarInfo;
    info2->name = varFirst->id->i;
    info2->declaredLine = 24;
    table->addVarInfo(info2);

    varFirstS->v = variableFirst;
    mainCompound->stmts->push_back(varFirstStmt);


    Statement *varSecondS = new Statement;
    Var *varSecond = new Var;
    varSecond->id = new Id;
    varSecond->id->i = new string("second");

    Variable *variableSecond = new Variable;
    variableSecond->v = varSecond;
    variableSecond->type = 'i';

    Stmt *varSecondStmt = new Stmt;
    varSecondStmt->type = 'v';
    varSecondStmt->statement = varSecondS;

    VarInfo *info3 = new VarInfo;
    info3->name = varSecond->id->i;
    info3->declaredLine = 25;
    table->addVarInfo(info3);

    varSecondS->v = variableSecond;
    mainCompound->stmts->push_back(varSecondStmt);


    Statement *varNextS = new Statement;
    Var *varNext = new Var;
    varNext->id = new Id;
    varNext->id->i = new string("next");

    Stmt *varNextStmt = new Stmt;
    varNextStmt->statement = varNextS;
    varNextStmt->type = 'v';

    VarInfo *info4 = new VarInfo;
    info4->name = varNext->id->i;
    info4->declaredLine = 26;
    table->addVarInfo(info4);

    Variable *variableNext = new Variable;
    variableNext->v = varNext;
    variableNext->type = 'i';

    varNextS->v = variableNext;
    mainCompound->stmts->push_back(varNextStmt);


    Statement *varCS = new Statement;
    Assign *varC = new Assign;
    varC->i = new Id;
    varC->i->i = new string("c");

    Variable *variableC = new Variable;
    variableC->v = new Var;
    variableC->type = 'a';
    variableC->v->a = varC;

    varCS->v = variableC;

    VarInfo *infoA = new VarInfo;
    infoA->declaredLine = 27;
    infoA->name = varC->i->i;
    table->addVarInfo(infoA);

    Stmt *varCStmt = new Stmt;
    varCStmt->statement = varCS;
    varCStmt->type = 'v';

    varC->e = new Expression;
    varC->e->expr = new Expr;
    varC->e->type = 's';
    varC->e->expr->e1 = new Simple;
    varC->e->expr->e1->e1 = new Simple1;
    varC->e->expr->e1->e1->t = new Term;
    varC->e->expr->e1->e1->t->t1 = new Term1;
    varC->e->expr->e1->e1->t->t1->f = new Factor;
    varC->e->expr->e1->e1->t->t1->f->con = new string("0");
    //varCS->a = varC;
    mainCompound->stmts->push_back(varCStmt);


    // print statement
    Statement *procCall = new Statement;
    procCall->c = new Call;
    procCall->c->name = new Id;
    procCall->c->name->i = new string("printst");
    procCall->c->params = new vector<Expression *>;

    Stmt *procCallStmt = new Stmt;
    procCallStmt->statement = procCall;
    procCallStmt->type = 'c';

    Expression *paramExpression = new Expression;
    Expr *paramExpr = new Expr;
    paramExpression->expr = paramExpr;
    paramExpression->type = 's';
    paramExpr->e1 = new Simple;
    paramExpr->e1->e1 = new Simple1;
    paramExpr->e1->e1->t = new Term;
    paramExpr->e1->e1->t->t1 = new Term1;
    paramExpr->e1->e1->t->t1->f = new Factor;
    paramExpr->e1->e1->t->t1->f->con = new string("enter the number of terms");

    procCall->c->params->push_back(paramExpression);
    mainCompound->stmts->push_back(procCallStmt);


    // getstatement
    Statement *getS = new Statement;
    getS->g = new Get;
    getS->g->id = new Id;
    getS->g->id->i = new string("n");

    Stmt *getStmt = new Stmt;
    getStmt->statement = getS;
    getStmt->type = 'g';

    mainCompound->stmts->push_back(getStmt);

    // while statement
    Statement *whileS = new Statement;
    whileS->w = new While;
    whileS->w->e = new Expression;
    whileS->w->e->expr = new Expr;
    whileS->w->e->type = 'e';
    whileS->w->s1 = new Compound;
    whileS->w->s1->startLine = 33;
    whileS->w->s1->endLine = 48;
    whileS->w->s1->stmts = new vector<Stmt *>;

    // boolean
    whileS->w->e->expr->e2 = new Expr2;
    whileS->w->e->expr->e2->e1 = new Simple;
    whileS->w->e->expr->e2->e2 = new Simple;
    whileS->w->e->expr->e2->op = new RelOp(LT);

    whileS->w->e->expr->e2->e1->e1 = new Simple1;
    whileS->w->e->expr->e2->e1->e1->t = new Term;
    whileS->w->e->expr->e2->e1->e1->t->t1 = new Term1;
    whileS->w->e->expr->e2->e1->e1->t->t1->f = new Factor;
    whileS->w->e->expr->e2->e1->e1->t->t1->f->id = new Id;
    whileS->w->e->expr->e2->e1->e1->t->t1->f->id->i = new string("first");

    whileS->w->e->expr->e2->e2->e1 = new Simple1;
    whileS->w->e->expr->e2->e2->e1->t = new Term;
    whileS->w->e->expr->e2->e2->e1->t->t1 = new Term1;
    whileS->w->e->expr->e2->e2->e1->t->t1->f = new Factor;
    whileS->w->e->expr->e2->e2->e1->t->t1->f->id = new Id;
    whileS->w->e->expr->e2->e2->e1->t->t1->f->id->i = new string("n");

    Statement *assign = new Statement;
    assign->a = new Assign;
    assign->a->i = new Id;
    assign->a->i->i = new string("c");


    assign->a->e = new Expression;
    assign->a->e->expr = new Expr;
    assign->a->e->type = 's';
    assign->a->e->expr->e1 = new Simple;
    assign->a->e->expr->e1->e2 = new Simple2;
    assign->a->e->expr->e1->e2->op = new AddOp(Plus);
    assign->a->e->expr->e1->e2->t1 = new Term;
    assign->a->e->expr->e1->e2->t1->t1 = new Term1;
    assign->a->e->expr->e1->e2->t1->t1->f = new Factor;
    assign->a->e->expr->e1->e2->t1->t1->f->id = new Id;
    assign->a->e->expr->e1->e2->t1->t1->f->id->i = new string("first");

    assign->a->e->expr->e1->e2->t2 = new Term;
    assign->a->e->expr->e1->e2->t2->t1 = new Term1;
    assign->a->e->expr->e1->e2->t2->t1->f = new Factor;
    assign->a->e->expr->e1->e2->t2->t1->f->id = new Id;
    assign->a->e->expr->e1->e2->t2->t1->f->id->i = new string("first");

    Stmt *assignStmt = new Stmt;
    assignStmt->statement = assign;
    assignStmt->type = 'a';

    whileS->w->s1->stmts->push_back(assignStmt);

    Stmt *whileStmt = new Stmt;
    whileStmt->statement = whileS;
    whileStmt->type = 'w';

    mainCompound->stmts->push_back(whileStmt);
    mainCompound->startLine = 19;
    mainCompound->endLine = 52;

    p->cs = mainCompound;

    return p;
}