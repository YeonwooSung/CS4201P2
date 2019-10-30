#include <string>
#include <vector>

using namespace std;

enum MulOp{Mul, Div, And};
enum AddOp{Plus, Minus, Or};
enum RelOp{Eq, NEq, LT, GT, LTE, GTE};

struct Compound;
union Factor;
union Simple;
struct Procedure;
union Var;
union Expr;

union Value {
    bool b;
    int i;
    string *s;
};

struct VarInfo {
    string *name;
    Value *val;
    char type; // 'b' for bool, 'i' for int, and 's' for string
    int declaredLine;
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
    Expr *e;
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
    Expr *e;
    Compound *s;
};

struct If2 {
    Expr *e;
    Compound *s1;
    Compound *s2;
};

union If {
    If1 *i1;
    If2 *i2;
};

struct While {
    Expr *e;
    Compound *s1;
};

struct Call
{
    Id *name;
    vector<Expr *> *params;
};

struct Variable {
    Var *v;
    char type;
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
    char type;
};

struct Compound 
{
    vector<Stmt *> *stmts;
    int startLine;
    int endLine;
};

struct Term1
{
    Factor *f;
};

struct Term2
{
    Factor *f1;
    MulOp *op;
    Factor *f2;
};

union Term
{
    Term1 *t1;
    Term2 *t2;
};

struct Expr2
{
    Simple *e1;
    RelOp *op;
    Simple *e2;
};

struct Simple1
{
    Term *t;
};

struct Simple2
{
    Term *t1;
    AddOp *op;
    Term *t2;
};

union Expr
{
    Simple *e1;
    Expr2 *e2;
};

union Simple
{
    Simple1 *e1;
    Simple2 *e2;
};


union Factor
{
    Id *id;
    string *con;
    Expr *e;
    Expr *note;
};

struct Procedure
{
    string *name;
    vector<Variable *> *params;
    Compound *cs;
    int startLine;
    int endLine;
};

Program *getExampleTree1(SymbolTable *table);
bool checkScopeOfAllProcedures(SymbolTable *table, vector<Procedure *> *procs);
