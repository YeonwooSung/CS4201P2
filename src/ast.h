#include <string>
#include <vector>

using namespace std;

enum MulOp{Mul, Div, And};
enum AddOp{Plus, Minus, Or};
enum RelOp{Eq, NEq, LT, GT, LTE, GTE};

struct Compound;
struct Factor;
struct Simple;
struct Procedure;
union Var;
union Expr;
struct Expression;


struct VarInfo {
    string *name;
    string *val;
    char type; // 'b' for bool, 'i' for int, and 's' for string
    int declaredLine;

    VarInfo() {
        name = NULL;
        val = NULL;
    }
};

/**
 * Class for the SymbolTable
 */
class SymbolTable {
    vector<VarInfo *> *list;
public:
    SymbolTable();
    ~SymbolTable();

    void addVarInfo(VarInfo *v);
    void remove(int i);
    vector<VarInfo *> *getList();
    int getSizeOfList();
};


struct Id {
    string *i;
};

struct Program
{
    string *name;
    vector<Procedure *> *ps;
    Compound *cs;
};

struct Assign {
    Id *i;
    Expression *e;
};

union Var {
    Id *id;
    Assign *a;
};

struct Print {
    string *s;
};

struct Get {
    Id *id;
};

struct If1 {
    Expression *e;
    Compound *s;
};

struct If2 {
    Expression *e;
    Compound *s1;
    Compound *s2;
};

union IfStmt {
    If1 *if1;
    If2 *if2;
};

struct If {
    IfStmt *i;
    char type; // '1' for If1, '2' for If2
};

struct While {
    Expression *e;
    Compound *s1;
};

struct Call {
    Id *name;
    vector<Expression *> *params;
};

struct Variable {
    Var *v;
    char type; // 'i' for Id, 'a' for Assign
};

union Statement {
    Variable *v;
    Print *p;
    Get *g;
    While *w;
    If *i;
    Assign *a;
    Call *c;
};

struct Stmt {
    Statement *statement;
    char type; // 'i' for If, 'a' for Assign, 'w' for While, 'p' for Print, 'g' for Get, 'v' for Var
};

struct Compound {
    vector<Stmt *> *stmts;
    int startLine;
    int endLine;
};

struct Term1 {
    Factor *f;
};

struct Term2 {
    Factor *f1;
    MulOp *op;
    Factor *f2;
};

struct Term {
    Term1 *t1;
    Term2 *t2;

    Term() {
        t1 = NULL;
        t2 = NULL;
    }
};

struct Expr2 {
    Simple *e1;
    RelOp *op;
    Simple *e2;
};

struct Simple1 {
    Term *t;
};

struct Simple2 {
    Term *t1;
    AddOp *op;
    Term *t2;
};

struct Expression {
    Expr *expr;
    char type; // 's' for Simple, 'e' for Expr2
};

union Expr {
    Simple *e1;
    Expr2 *e2;
};

struct Simple {
    Simple1 *e1;
    Simple2 *e2;

    Simple() {
        e1 = NULL;
        e2 = NULL;
    }
};

struct Factor {
    Id *id;
    string *con;
    Expression *e;
    Expression *note;

    Factor() {
        id = NULL;
        con = NULL;
        e = NULL;
        note = NULL;
    }
};

struct Procedure {
    string *name;
    vector<Variable *> *params;
    Compound *cs;
    int startLine;
    int endLine;
};

Program *getExampleTree1(SymbolTable *table);
bool checkScopeOfAllProcedures(SymbolTable *table, vector<Procedure *> *procs);
bool checkScopeOfStmts(SymbolTable *table, vector<Stmt *> *stmts, vector<Variable *> *params, int startLine, int endLine);
bool checkType(vector<Stmt *> *stmts);
