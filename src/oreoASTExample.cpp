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
        //TODO remove instances in the vector
        delete list;
    }
}

/**
 * Adds the VarInfo instace to the vector.
 * @param {v} new instance
 */
void SymbolTable::addVarInfo(VarInfo *v) {
    if (list != NULL) {
        list->insert(list->begin(), v);
    } else {
        list = new vector<VarInfo *>;
        list->insert(list->begin(), v);
    }
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

    Procedure *proc = new Procedure;

    string *procName = new string("printst");
    
    vector <Var*>* procParams = new vector<Var*>;


    Var *procParam = new Var;
    procParam->id = new Id;
    procParam->id->i = new string( "str" );
    VarInfo *info1 = new VarInfo;
    info1->name = procParam->id->i;
    info1->declaredLine = 15;
    table->addVarInfo(info1);

    procParams->push_back(procParam);


    Compound *procCompound = new Compound;
    vector <Statement *> *procStatements = new vector<Statement *>;

    Statement *procStatement = new Statement;

    procStatement->p = new Print;
    procStatement->p->s = new string("str");

    proc->cs = new Compound;
    proc->cs->stmts =  new vector<Statement *>;
    proc->cs->stmts->push_back(procStatement);

    // main function
    Compound *mainCompound = new Compound;
    mainCompound->stmts = new vector<Statement *>;

    // variable declarations
    Statement *varNS = new Statement;
    Var *varN = new Var;
    varN->id = new Id;
    varN->id->i = new string("n");

    VarInfo *info2 = new VarInfo;
    info2->name = varN->id->i;
    info2->declaredLine = 22;
    table->addVarInfo(info2);

    varNS->v = varN;
    mainCompound->stmts->push_back(varNS);


    Statement *varFirstS = new Statement;
    Var *varFirst = new Var;
    varFirst->id = new Id;
    varFirst->id->i = new string("first");

    VarInfo *info3 = new VarInfo;
    info3->name = varFirst->id->i;
    info3->declaredLine = 24;
    table->addVarInfo(info3);

    varFirstS->v = varFirst;
    mainCompound->stmts->push_back(varFirstS);


    Statement *varSecondS = new Statement;
    Var *varSecond = new Var;
    varSecond->id = new Id;
    varSecond->id->i = new string("second");

    VarInfo *info4 = new VarInfo;
    info4->name = varSecond->id->i;
    info4->declaredLine = 25;
    table->addVarInfo(info4);

    varSecondS->v = varSecond;
    mainCompound->stmts->push_back(varSecondS);


    Statement *varNextS = new Statement;
    Var *varNext = new Var;
    varNext->id = new Id;
    varNext->id->i = new string("next");

    VarInfo *info5 = new VarInfo;
    info5->name = varNext->id->i;
    info5->declaredLine = 26;
    table->addVarInfo(info5);

    varNextS->v = varNext;
    mainCompound->stmts->push_back(varNextS);


    Statement *varCS = new Statement;
    Assign *varC = new Assign;
    varC->i = new Id;
    varC->i->i = new string("c");

    VarInfo *info6 = new VarInfo;
    info6->name = varC->i->i;
    info6->declaredLine = 28;
    table->addVarInfo(info6);

    varC->e = new Expr;
    varC->e->e1 = new Simple;
    varC->e->e1->e1 = new Simple1;
    varC->e->e1->e1->t = new Term;
    varC->e->e1->e1->t->t1 = new Term1;
    varC->e->e1->e1->t->t1->f = new Factor;
    varC->e->e1->e1->t->t1->f->con = new string("0");
    varCS->a = varC;
    mainCompound->stmts->push_back(varCS);


    // print statement
    Statement *procCall = new Statement;
    procCall->c = new Call;
    procCall->c->name = new Id;
    procCall->c->name->i = new string("printst");

    procCall->c->params = new vector<Expr *>;

    Expr *paramExpr = new Expr;
    paramExpr->e1 = new Simple;
    paramExpr->e1->e1 = new Simple1;
    paramExpr->e1->e1->t = new Term;
    paramExpr->e1->e1->t->t1 = new Term1;
    paramExpr->e1->e1->t->t1->f = new Factor;
    paramExpr->e1->e1->t->t1->f->con = new string("enter the number of terms");

    procCall->c->params->push_back(paramExpr);
    mainCompound->stmts->push_back(procCall);


    // getstatement
    Statement *getS = new Statement;
    getS->g = new Get;
    getS->g->id = new Id;
    getS->g->id->i = new string("n");
    mainCompound->stmts->push_back(getS);

    // while statement
    Statement *whileS = new Statement;
    whileS->w = new While;
    whileS->w->e = new Expr;
    whileS->w->s1 = new Compound;

    // boolean
    whileS->w->e->e2 = new Expr2;
    whileS->w->e->e2->e1 = new Simple;
    whileS->w->e->e2->e2 = new Simple;
    whileS->w->e->e2->op = new RelOp(LT);

    whileS->w->e->e2->e1->e1 = new Simple1;
    whileS->w->e->e2->e1->e1->t = new Term;
    whileS->w->e->e2->e1->e1->t->t1 = new Term1;
    whileS->w->e->e2->e1->e1->t->t1->f = new Factor;
    whileS->w->e->e2->e1->e1->t->t1->f->id = new Id;
    whileS->w->e->e2->e1->e1->t->t1->f->id->i = new string("first");


    whileS->w->e->e2->e2->e1 = new Simple1;
    whileS->w->e->e2->e2->e1->t = new Term;
    whileS->w->e->e2->e2->e1->t->t1 = new Term1;
    whileS->w->e->e2->e2->e1->t->t1->f = new Factor;
    whileS->w->e->e2->e2->e1->t->t1->f->id = new Id;
    whileS->w->e->e2->e2->e1->t->t1->f->id->i = new string("n");

    whileS->w->s1 = new Compound;
    whileS->w->s1->stmts = new vector <Statement*>;

    Statement *assign = new Statement;
    assign->a = new Assign;
    assign->a->i = new Id;
    assign->a->i->i = new string("c");


    assign->a->e = new Expr;
    assign->a->e->e1 = new Simple;
    assign->a->e->e1->e2 = new Simple2;
    assign->a->e->e1->e2->op = new AddOp(Plus);
    assign->a->e->e1->e2->t1 = new Term;
    assign->a->e->e1->e2->t1->t1 = new Term1;
    assign->a->e->e1->e2->t1->t1->f = new Factor;
    assign->a->e->e1->e2->t1->t1->f->id = new Id;
    assign->a->e->e1->e2->t1->t1->f->id->i = new string("first");


    assign->a->e->e1->e2->t2 = new Term;
    assign->a->e->e1->e2->t2->t1 = new Term1;
    assign->a->e->e1->e2->t2->t1->f = new Factor;
    assign->a->e->e1->e2->t2->t1->f->id = new Id;
    assign->a->e->e1->e2->t2->t1->f->id->i = new string("first");

    whileS->w->s1->stmts->push_back(assign);

    mainCompound->stmts->push_back(whileS);

    return p;
}
