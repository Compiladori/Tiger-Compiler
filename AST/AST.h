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
    Symbol(std::string name);
    std::string get_name();
};



/** Position **/
class Position {
    int pos;
public:
    Position (int pos);
};



/** Types **/
class Type {
public:
    virtual void print();
};

class TypeField {
    Symbol id;
    Symbol type_id;
public:
    TypeField(Symbol id, Symbol type_id);
};

class NameType : public Type {
    Symbol type_id;
public:
    NameType (Symbol type_id);
};

class RecordType : public Type {
    TypeFieldList *tyfields;
public:
    RecordType (TypeFieldList *tyfields);
};

class ArrayType : public Type {
    Symbol type_id;
public:
    ArrayType (Symbol type_id);
};



/** Variables **/
class Variable {
public:
    virtual void print();
};

class SimpleVar : public Variable {
    Symbol id;
public:
    SimpleVar (Symbol symbol);
};

class FieldVar : public Variable {
    Variable *var;
    Symbol id;
public:
    FieldVar (Variable *var, Symbol id);
};

class SubscriptVar : public Variable {
    Variable *var;
    Expression *exp;
public:
    SubscriptVar (Variable *var, Expression *exp);
};



/** Expressions **/
class Expression {
    Position pos;
public:
    virtual void print();
    Position get_position();
};

class VarExp : public Expression {
    Variable *var;
public:
    VarExp (Variable &var, Position pos);
};

class UnitExp : public Expression {
public:
    UnitExp (Position pos);
};

class NilExp : public Expression {
public:
    NilExp (Position pos);
};

class IntExp : public Expression {
    int value;
public:
    IntExp (int value, Position pos);
    int get_value();
};

class StringExp : public Expression {
    std::string value;
public:
    StringExp (std::string value, Position pos);
    std::string get_value();
};

class CallExp : public Expression {
    Symbol func;
    ExpressionList *exp_list;
public:
    CallExp (Symbol func, ExpressionList *exp_list);
};

class OpExp : public Expression {
    Expression *left;
    Operation oper;
    Expression *right;
public:
    OpExp (Expression *left, Operation oper, Expression *right);
};

class RecordExp : public Expression {
    VariableList *fields;
    Type *ty;
public:
    RecordExp (VariableList *fields, Type *ty, Position pos);
};

class SeqExp : public Expression {
    ExpressionList *exp_list;
public:
    SeqExp (ExpressionList *exp_list, Position pos);
};

class AssignExp : public Expression {
    Variable *var;
    Expression *exp;
public:
    AssignExp (Variable *var, Expression *exp, Position pos);
};

class IfExp : public Expression {
    Expression *test;
    Expression *then;
    Expression *otherwise;
public:
    IfExp (Expression *test, Expression *then, Expression *otherwise, Position pos);
};

class WhileExp : public Expression {
    Expression *test;
    Expression *body;
public:
    WhileExp (Expression *test, Expression *body, Position pos);
};

class ForExp : public Expression {
    Variable *var;
    bool escape;
    Expression *lo;
    Expression *hi;
    Expression *body;
public:
    ForExp (Variable *var, bool escape, Expression *lo, Expression *hi, Expression *body, Position pos);
    bool get_escape();
};

class LetExp : public Expression {
    DeclarationList *decs;
    Expression *body;
public:
    LetExp (DeclarationList *decs, Expression *body, Position pos);
};

class BreakExp : public Expression {
public:
    BreakExp (Position pos);
};

class ArrayExp : public Expression {
    Symbol ty;
    Expression *size;
    Expression *init;
public:
    ArrayExp (Symbol ty, Expression *size, Expression *init, Position pos);
};



/** Declarations **/
class Declaration {
public:
    virtual void print();
};

class VarDec : public Declaration {
    Symbol id;
    bool escape;
    Symbol type_id;
    Expression *exp;
public:
    VarDec(Symbol id, bool escape, Expression *exp);
    VarDec(Symbol id, bool escape, Symbol type_id, Expression *exp);
};

class TypeDec : public Declaration {
    Symbol type_id;
    Type *ty;
public:
    TypeDec(Symbol type_id, Type *ty);
};

class FunDec : public Declaration {
    Symbol id;
    TypeFieldList *tyfields;
    Symbol type_id;
    Expression *exp;
public:
    FunDec(Symbol id, TypeFieldList *tyfields, Expression *exp);
    FunDec(Symbol id, TypeFieldList *tyfields, Symbol type_id, Expression *exp);
};



};

#endif
