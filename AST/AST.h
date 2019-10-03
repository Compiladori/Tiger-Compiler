#ifndef __AST_H__
#define __AST_H__

#include <iostream>
#include <string>
#include <vector>
#include <memory>

namespace ast {



/** Forward declarations **/
class Symbol;
class Position;
class Type;
class TypeField;
class RecordField;
class Variable;
class Expression;
class Declaration;
class GroupedDeclarations;


typedef enum {VarDK, FunDK, TypeDK, NoDK} DeclarationKind;

template <class T>
class GenericList : public std::vector<std::unique_ptr<T>> {
public:
    GenericList()     : std::vector<std::unique_ptr<T>>() {}
    GenericList(T *e) : GenericList() { this->push_back(e); }
    void push_back(T *e){ this->std::vector<std::unique_ptr<T>>::push_back(std::move(std::unique_ptr<T>(e))); }
};

typedef GenericList<TypeField> TypeFieldList;
typedef GenericList<Variable> VariableList;
typedef GenericList<Expression> ExpressionList;
typedef GenericList<Declaration> DeclarationList;
typedef GenericList<RecordField> RecordFieldList;



/** Operations **/
typedef enum {Plus, Minus, Times, Divide, Eq, Neq, Lt, Le, Gt, Ge} Operation;



/** Symbols (Identifiers) **/
class Symbol {
    std::string name;
public:
    Symbol(std::string name) : name(name) {}
    std::string getName(){ return name; }
    void print() { std::cout << "Symbol " << name << std::endl; }
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
    std::unique_ptr<Symbol> id, type_id;
public:
    TypeField(Symbol *id, Symbol *type_id) : id(id), type_id(type_id) {}
    void print(){   std::cout << "TypeField ("; id -> print();
                    std::cout <<") ("; type_id -> print(); std::cout << ")"; }
};

class RecordField {
    std::unique_ptr<Symbol> id;
    std::unique_ptr<Expression> exp;
public:
    RecordField(Symbol *id, Expression *exp) : id(id), exp(exp) {}
    void print(){ std::cout << "RecordField ("; id -> print(); std::cout <<") ("; exp -> print(); std::cout << ")"; }
};

class NameType : public Type {
    std::unique_ptr<Symbol> type_id;
public:
    NameType (Symbol *type_id) : type_id(type_id) {}
    void print(){ std::cout << "NameType ("; type_id -> print(); std::cout << ")";}
};

class RecordType : public Type {
    std::unique_ptr<TypeFieldList> tyfields;
public:
    RecordType (TypeFieldList *tyfields) : tyfields(tyfields) {}
    void print(){ std::cout << "RecordType ("; tyfields -> print(); std::cout << ")";}
};

class ArrayType : public Type {
    std::unique_ptr<Symbol> type_id;
public:
    ArrayType (Symbol *type_id) : type_id(type_id) {}
    void print(){ std::cout << "ArrayType ("; type_id -> print(); std::cout << ")"; }
};



/** Variables **/
class Variable {
public:
    virtual void print() = 0;
    virtual std::string getName() = 0;
};

class SimpleVar : public Variable {
    std::unique_ptr<Symbol> id;
public:
    SimpleVar (Symbol *id) : id(id) {}
    std::string getName(){ return id->getName(); };
    void print(){ std::cout << "SimpleVar ("; id -> print(); std::cout << ")"; }
};

class FieldVar : public Variable {
    std::unique_ptr<Variable> var;
    std::unique_ptr<Symbol> id;
public:
    FieldVar (Variable *var, Symbol *id) : var(var), id(id) {}
    std::string getName(){ return var->getName(); };
    void print(){ std::cout << "FieldVar ("; var -> print(); std::cout << ") ("; id -> print(); std::cout << ")"; }
};

class SubscriptVar : public Variable {
    std::unique_ptr<Variable> var;
    std::unique_ptr<Expression> exp;
public:
    SubscriptVar (Variable *var, Expression *exp) : var(var), exp(exp) {}
    std::string getName(){ return var->getName(); };
    void print(){ std::cout << "SubscriptVar ("; var -> print(); std::cout << ") ("; exp -> print(); std::cout << ")"; }
};



/** Expressions **/
class Expression {
    Position pos;
public:
    Expression(Position pos) : pos(pos) {}
    Position getPosition(){ return pos; }

    virtual void print() = 0;
};

class VarExp : public Expression {
    std::unique_ptr<Variable> var;
public:
    VarExp (Variable *var, Position pos) : Expression(pos), var(var) {}
    void print(){ std::cout << "VarExp (" << pos -> int << " )"; }
};

class UnitExp : public Expression {
public:
    UnitExp (Position pos) : Expression(pos) {}
    void print(){ std::cout << "UnitExp (" << pos -> int << " )"; }
};

class NilExp : public Expression {
public:
    NilExp (Position pos) : Expression(pos) {}
    void print(){ std::cout << "NilExp ( )"; }
};

template<class T>
class GenericValueExp : public Expression {
    T value;
public:
    GenericValueExp(T value, Position pos) : Expression(pos), value(value) {}
    T getValue(){ return value; }
    void print(){}
};

typedef GenericValueExp<int> IntExp;
typedef GenericValueExp<double> RealExp;
typedef GenericValueExp<std::string> StringExp;

class CallExp : public Expression {
    std::unique_ptr<Symbol> func;
    std::unique_ptr<ExpressionList> exp_list;
public:
    CallExp (Symbol *func, ExpressionList *exp_list, Position pos) : Expression(pos), func(func), exp_list(exp_list) {}
    void print(){ std::cout << "CallExp ("; func -> print(); std::cout << ") ("; exp_list -> print(); std::cout << ")"; }
};
/*
operToText(Operation op){



}
*/

class OpExp : public Expression {
    std::unique_ptr<Expression> left;
    Operation oper;
    std::unique_ptr<Expression> right;
public:
    OpExp (Expression *left, Operation oper, Expression *right, Position pos) : Expression(pos), left(left), oper(oper), right(right) {}
    void print(){ std::cout << "OpExp ("; left -> print(); std::cout << ") " << oper << "( "; right -> print(); std::cout << ")"; }
};

class RecordExp : public Expression {
    std::unique_ptr<RecordFieldList> fields;
    std::unique_ptr<Symbol> type_id;
public:
    RecordExp (RecordFieldList *fields, Symbol *type_id, Position pos) : Expression(pos), fields(fields), type_id(type_id) {}
    void print(){ std::cout << "RecordExp ("; fields -> print(); std::cout << ") ("; type_id -> print(); std::cout << ")"; }
};

class SeqExp : public Expression {
    std::unique_ptr<ExpressionList> exp_list;
public:
    SeqExp (ExpressionList *exp_list, Position pos) : Expression(pos), exp_list(exp_list) {}
    void print(){}
};

class AssignExp : public Expression {
    std::unique_ptr<Variable> var;
    std::unique_ptr<Expression> exp;
public:
    AssignExp (Variable *var, Expression *exp, Position pos) : Expression(pos), var(var), exp(exp) {}
    void print(){}
};

class IfExp : public Expression {
    std::unique_ptr<Expression> test, then, otherwise;
public:
    IfExp (Expression *test, Expression *then, Position pos) : Expression(pos), test(test), then(then), otherwise(nullptr) {}
    IfExp (Expression *test, Expression *then, Expression *otherwise, Position pos) : Expression(pos), test(test), then(then), otherwise(otherwise) {}
    void print(){}
};

class WhileExp : public Expression {
    std::unique_ptr<Expression> test, body;
public:
    WhileExp (Expression *test, Expression *body, Position pos) : Expression(pos), test(test), body(body) {}
    void print(){}
};

class ForExp : public Expression {
    std::unique_ptr<Variable> var;
    bool escape;
    std::unique_ptr<Expression> lo, hi, body;
public:
    ForExp (Variable *var, bool escape, Expression *lo, Expression *hi, Expression *body, Position pos) : Expression(pos), var(var), escape(escape), lo(lo), hi(hi), body(body) {}
    bool getEscape(){ return escape; };
    void print(){}
};

class LetExp : public Expression {
    std::unique_ptr<GroupedDeclarations> decs;
    std::unique_ptr<Expression> body;
public:
    LetExp (GroupedDeclarations *decs, Expression *body, Position pos) : Expression(pos), decs(decs), body(body) {}
    void print(){}
};

class BreakExp : public Expression {
public:
    BreakExp (Position pos) : Expression(pos) {}
    void print(){}
};

class ArrayExp : public Expression {
    std::unique_ptr<Symbol> ty;
    std::unique_ptr<Expression> size, init;
public:
    ArrayExp (Symbol *ty, Expression *size, Expression *init, Position pos) : Expression(pos), ty(ty), size(size), init(init) {}
    void print(){}
};



/** Declarations **/
class Declaration {
    DeclarationKind DK;
public:
    Declaration () : DK(DeclarationKind::NoDK) {}
    Declaration (DeclarationKind DK) : DK(DK) {}
    DeclarationKind getKind(){ return DK; }

    virtual void print() = 0;
};

class VarDec : public Declaration {
    std::unique_ptr<Symbol> id;
    bool escape;
    std::unique_ptr<Symbol> type_id;
    std::unique_ptr<Expression> exp;
public:
    VarDec(Symbol *id, bool escape, Expression *exp) : Declaration(DeclarationKind::VarDK), id(id), escape(escape), type_id(nullptr), exp(exp) {}
    VarDec(Symbol *id, bool escape, Symbol *type_id, Expression *exp) : Declaration(DeclarationKind::VarDK), id(id), escape(escape), type_id(type_id), exp(exp) {}

    bool getEscape(){ return escape; }
    void print(){}
};

class TypeDec : public Declaration {
    std::unique_ptr<Symbol> type_id;
    std::unique_ptr<Type> ty;
public:
    TypeDec(Symbol *type_id, Type *ty) : Declaration(DeclarationKind::TypeDK), type_id(type_id), ty(ty) {}

    void print(){}
};

class FunDec : public Declaration {
    std::unique_ptr<Symbol> id;
    std::unique_ptr<TypeFieldList> tyfields;
    std::unique_ptr<Symbol> type_id;
    std::unique_ptr<Expression> exp;
public:
    FunDec(Symbol *id, TypeFieldList *tyfields, Expression *exp) : Declaration(DeclarationKind::FunDK), id(id), tyfields(tyfields), type_id(nullptr), exp(exp) {}
    FunDec(Symbol *id, TypeFieldList *tyfields, Symbol *type_id, Expression *exp) : Declaration(DeclarationKind::FunDK), id(id), tyfields(tyfields), type_id(type_id), exp(exp) {}

    void print(){}
};

class GroupedDeclarations : public GenericList<DeclarationList> {
public:
    void appendElement(Declaration *dec){
        if(this->empty() or dec->getKind() == DeclarationKind::VarDK) {
            // Create a new group if we had no group to join to, or we're a variable declaration
            this->push_back(new DeclarationList(dec));
        } else {
            // Join last group if its Kind matches
            if(this->back()->back()->getKind() == dec->getKind()) this->back()->push_back(dec);
            else                                                  this->push_back(new DeclarationList(dec));
        }
    }
};



};

#endif
