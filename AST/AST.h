#ifndef __AST_H__
#define __AST_H__

#include <string>
#include <vector>


namespace ast {


/** Incomplete type declarations **/
class Symbol;
class Position;
class Type;
class TypeField;
class Variable;
class Expression;
class Declaration;

typedef std::vector<TypeField*> TypeFieldList;
typedef std::vector<Variable*> VariableList;
typedef std::vector<Expression*> ExpressionList;
typedef std::vector<Declaration*> DeclarationList;



/** Operations **/
typedef enum {Plus, Minus, Times, Divide, Eq, Neq, Lt, Le, Gt, Ge} Operation;



/** Symbols (Identifiers) **/
class Symbol {
    std::string name;
public:
    Symbol(std::string name) : name(name) {}
    Symbol(char *name) : name(std::string(name)) {}
    std::string getName();
};



/** Position **/
class Position {
    int pos;
public:
    Position (int pos) : pos(pos) {}
};



/** Types **/
class Type {
public:
    virtual void print() = 0;
};

class TypeField {
    Symbol *id;
    Symbol *type_id;
public:
    TypeField(Symbol *id, Symbol *type_id) : id(id), type_id(type_id) {}
};

class NameType : public Type {
    Symbol *type_id;
public:
    NameType (Symbol *type_id) : type_id(type_id) {}
};

class RecordType : public Type {
    TypeFieldList *tyfields;
public:
    RecordType (TypeFieldList *tyfields) : tyfields(tyfields) {}
};

class ArrayType : public Type {
    Symbol *type_id;
public:
    ArrayType (Symbol *type_id) : type_id(type_id) {}
};



/** Variables **/
class Variable {
public:
    virtual void print() = 0;
};

class SimpleVar : public Variable {
    Symbol *id;
public:
    SimpleVar (Symbol *id) : id(id) {}
};

class FieldVar : public Variable {
    Variable *var;
    Symbol *id;
public:
    FieldVar (Variable *var, Symbol *id) : var(var), id(id) {}
};

class SubscriptVar : public Variable {
    Variable *var;
    Expression *exp;
public:
    SubscriptVar (Variable *var, Expression *exp) : var(var), exp(exp) {}
};



/** Expressions **/
class Expression {
    Position *pos;
public:
    Expression(Position *pos) : pos(pos) {}
    virtual void print() = 0;
    Position getPosition();
};

class VarExp : public Expression {
    Variable *var;
public:
    VarExp (Variable *var, Position *pos) : var(var), Expression(pos) {}
};

class UnitExp : public Expression {
public:
    UnitExp (Position *pos) : Expression(pos) {}
};

class NilExp : public Expression {
public:
    NilExp (Position *pos) : Expression(pos) {}
};

template<class T>
class GenericValueExp : public Expression {
    T value;
public:
    GenericValueExp(T value, Position *pos) : value(value), Expression(pos) {}
    T getValue();
};

typedef GenericValueExp<int> IntExp;
typedef GenericValueExp<double> RealExp;
typedef GenericValueExp<std::string> StringExp;

class CallExp : public Expression {
    Symbol func;
    ExpressionList *exp_list;
public:
    CallExp (Symbol func, ExpressionList *exp_list, Position *pos) : func(func), exp_list(exp_list), Expression(pos) {}
};

class OpExp : public Expression {
    Expression *left;
    Operation oper;
    Expression *right;
public:
    OpExp (Expression *left, Operation oper, Expression *right, Position *pos) : left(left), oper(oper), right(right), Expression(pos) {}
};

class RecordExp : public Expression {
    VariableList *fields;
    Type *ty;
public:
    RecordExp (VariableList *fields, Type *ty, Position *pos) : fields(fields), ty(ty), Expression(pos) {}
};

class SeqExp : public Expression {
    ExpressionList *exp_list;
public:
    SeqExp (ExpressionList *exp_list, Position *pos) : exp_list(exp_list), Expression(pos) {}
};

class AssignExp : public Expression {
    Variable *var;
    Expression *exp;
public:
    AssignExp (Variable *var, Expression *exp, Position *pos) : var(var), exp(exp), Expression(pos) {}
};

class IfExp : public Expression {
    Expression *test;
    Expression *then;
    Expression *otherwise;
public:
    IfExp (Expression *test, Expression *then, Position *pos) : test(test), then(then), otherwise(nullptr), Expression(pos) {}
    IfExp (Expression *test, Expression *then, Expression *otherwise, Position *pos) : test(test), then(then), otherwise(otherwise), Expression(pos) {}
};

class WhileExp : public Expression {
    Expression *test;
    Expression *body;
public:
    WhileExp (Expression *test, Expression *body, Position *pos) : test(test), body(body), Expression(pos) {}
};

class ForExp : public Expression {
    Variable *var;
    bool escape;
    Expression *lo;
    Expression *hi;
    Expression *body;
public:
    ForExp (Variable *var, bool escape, Expression *lo, Expression *hi, Expression *body, Position *pos) : var(var), escape(escape), lo(lo), hi(hi), body(body), Expression(pos) {}
    bool getEscape();
};

class LetExp : public Expression {
    DeclarationList *decs;
    Expression *body;
public:
    LetExp (DeclarationList *decs, Expression *body, Position *pos) : decs(decs), body(body), Expression(pos) {}
};

class BreakExp : public Expression {
public:
    BreakExp (Position *pos) : Expression(pos) {}
};

class ArrayExp : public Expression {
    Symbol *ty;
    Expression *size;
    Expression *init;
public:
    ArrayExp (Symbol *ty, Expression *size, Expression *init, Position *pos) : ty(ty), size(size), init(init), Expression(pos) {}
};



/** Declarations **/
class Declaration {
public:
    virtual void print() = 0;
};

class VarDec : public Declaration {
    Symbol *id;
    bool escape;
    Symbol *type_id;
    Expression *exp;
public:
    VarDec(Symbol *id, bool escape, Expression *exp) : id(id), escape(escape), type_id(nullptr), exp(exp) {}
    VarDec(Symbol *id, bool escape, Symbol *type_id, Expression *exp) : id(id), escape(escape), type_id(type_id), exp(exp) {}
    bool getEscape();
};

class TypeDec : public Declaration {
    Symbol *type_id;
    Type *ty;
public:
    TypeDec(Symbol *type_id, Type *ty) : type_id(type_id), ty(ty) {}
};

class FunDec : public Declaration {
    Symbol *id;
    TypeFieldList *tyfields;
    Symbol *type_id;
    Expression *exp;
public:
    FunDec(Symbol *id, TypeFieldList *tyfields, Expression *exp) : id(id), tyfields(tyfields), type_id(nullptr), exp(exp) {}
    FunDec(Symbol *id, TypeFieldList *tyfields, Symbol *type_id, Expression *exp) : id(id), tyfields(tyfields), type_id(type_id), exp(exp) {}
};



};

#endif
