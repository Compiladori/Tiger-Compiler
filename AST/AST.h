#ifndef __AST_H__
#define __AST_H__

/***
 * Abstract Syntax Tree (AST)
 *
 * Described in Chapter 4 Appel C (2004)
 *
 * Every abstraction of the AST is constructed by raw pointers, taking ownership over them.
 * This is a special case to work along with the usual versions of Bison and Flex.
 * ***/

#include <iostream>
#include <string>
#include <deque>
#include <memory>
#include <functional>
#include "../Utility/utility.h"

namespace ast {

/**
 * Forward declarations
 * **/
class Symbol;
class Position;
class Type;
class TypeField;
class RecordField;
class Variable;
class Expression;
class Declaration;
class GroupedDeclarations;

using TypeList        = util::GenericList<Type>;
using TypeFieldList   = util::GenericList<TypeField>;
using VariableList    = util::GenericList<Variable>;
using ExpressionList  = util::GenericList<Expression>;
using DeclarationList = util::GenericList<Declaration>;
using RecordFieldList = util::GenericList<RecordField>;

/**
 * Operations
 * **/
enum Operation {Plus, Minus, Times, Divide, Eq, Neq, Lt, Le, Gt, Ge};

/**
 * Symbols (Identifiers)
 * **/
struct Symbol {
    std::string name;

    Symbol ()                 : name()     {}
    Symbol (const char* name) : name(name) {}
    Symbol (std::string name) : name(name) {}

    bool operator==(const Symbol& s) const { return name == s.name; }

    void print() const { std::cout << "Symbol " << name; }
};

struct SymbolHasher{
    std::size_t operator()(const Symbol& s) const {
        return std::hash<std::string>()(s.name);
    }
};


/**
 * Position
 * **/
struct Position {
    int pos;

    Position (int pos) : pos(pos) {}
    std::string to_string() { return std::to_string(pos); }
    void print() const { std::cout << pos; }
};

/**
 * Types
 * **/
struct Type {
    Position pos;
    Type(Position pos) : pos(pos) {}
    virtual ~Type() = default;
    virtual void print() const = 0;
};

struct TypeField {
    std::unique_ptr<Symbol> id, type_id;
    bool escape = true;

    TypeField (Symbol *id, Symbol *type_id) : id(id), type_id(type_id) {}
    void print() const;
};


struct NameType : public Type {
    std::unique_ptr<Symbol> type_id;

    NameType (Symbol *type_id, Position pos) : Type(pos), type_id(type_id) {}
    void print() const;
};

struct RecordType : public Type {
    std::unique_ptr<TypeFieldList> tyfields;

    RecordType (TypeFieldList *tyfields, Position pos) : Type(pos), tyfields(tyfields) {}
    void print() const;
};

struct ArrayType : public Type {
    std::unique_ptr<Symbol> type_id;

    ArrayType (Symbol *type_id, Position pos) : Type(pos), type_id(type_id) {}
    void print() const;
};

/**
 * Variables
 * **/
struct Variable {
    virtual ~Variable() = default;
    Position pos;
    Variable(Position pos) : pos(pos) {}
    virtual void print() const = 0;
};

struct SimpleVar : public Variable {
    std::unique_ptr<Symbol> id;

    SimpleVar (Symbol *id, Position pos) : Variable(pos), id(id) {}
    void print() const;
};

struct FieldVar : public Variable {
    std::unique_ptr<Variable> var;
    std::unique_ptr<Symbol> id;

    FieldVar (Variable *var, Symbol *id, Position pos) : Variable(pos), var(var), id(id) {}
    void print() const;
};

struct SubscriptVar : public Variable {
    std::unique_ptr<Variable> var;
    std::unique_ptr<Expression> exp;

    SubscriptVar (Variable *var, Expression *exp, Position pos) : Variable(pos), var(var), exp(exp) {}
    void print() const;
};

/**
 * Expressions
 * **/
struct Expression {
    Position pos;

    Expression(Position pos) : pos(pos) {}

    virtual ~Expression() = default;
    virtual void print() const = 0;
};

struct VarExp : public Expression {
    std::unique_ptr<Variable> var;

    VarExp (Variable *var, Position pos) : Expression(pos), var(var) {}
    void print() const;
};

struct UnitExp : public Expression {
    UnitExp (Position pos) : Expression(pos) {}
    void print() const;
};

struct NilExp : public Expression {
    NilExp (Position pos) : Expression(pos) {}
    void print() const;
};

template<class T>
struct GenericValueExp : public Expression {
    T value;

    GenericValueExp(T value, Position pos) : Expression(pos), value(value) {}

    void print() const { std::cout << "ValueExp (" << value << ")"; }
};

using IntExp    = GenericValueExp<int>;
using StringExp = GenericValueExp<std::string>;

struct CallExp : public Expression {
    std::unique_ptr<Symbol> func;
    std::unique_ptr<ExpressionList> exp_list;

    CallExp (Symbol *func, ExpressionList *exp_list, Position pos) : Expression(pos), func(func), exp_list(exp_list) {}
    void print() const;
};

struct OpExp : public Expression {
    std::unique_ptr<Expression> left;
    Operation oper;
    std::unique_ptr<Expression> right;

    OpExp (Expression *left, Operation oper, Expression *right, Position pos) : Expression(pos), left(left), oper(oper), right(right) {}
    void print() const;
};

struct RecordField {
    std::unique_ptr<Symbol> id;
    std::unique_ptr<Expression> exp;

    RecordField (Symbol *id, Expression *exp) : id(id), exp(exp) {}
    void print() const;
};

struct RecordExp : public Expression {
    std::unique_ptr<RecordFieldList> fields;
    std::unique_ptr<Symbol> type_id;

    RecordExp (RecordFieldList *fields, Symbol *type_id, Position pos) : Expression(pos), fields(fields), type_id(type_id) {}
    void print() const;
};

struct SeqExp : public Expression {
    std::unique_ptr<ExpressionList> exp_list;

    SeqExp (ExpressionList *exp_list, Position pos) : Expression(pos), exp_list(exp_list) {}
    void print() const;
};

struct AssignExp : public Expression {
    std::unique_ptr<Variable> var;
    std::unique_ptr<Expression> exp;

    AssignExp (Variable *var, Expression *exp, Position pos) : Expression(pos), var(var), exp(exp) {}
    void print() const;
};

struct IfExp : public Expression {
    std::unique_ptr<Expression> test, then, otherwise;

    IfExp (Expression *test, Expression *then, Position pos) : Expression(pos), test(test), then(then), otherwise() {}
    IfExp (Expression *test, Expression *then, Expression *otherwise, Position pos) : Expression(pos), test(test), then(then), otherwise(otherwise) {}
    void print() const;
};

struct WhileExp : public Expression {
    std::unique_ptr<Expression> test, body;

    WhileExp (Expression *test, Expression *body, Position pos) : Expression(pos), test(test), body(body) {}
    void print() const;
};

struct ForExp : public Expression {
    std::unique_ptr<Symbol> var;
    std::unique_ptr<Expression> lo, hi, body;
    bool escape = false;

    ForExp (Symbol *var, Expression *lo, Expression *hi, Expression *body, Position pos) : Expression(pos), var(var), lo(lo), hi(hi), body(body) {}

    void print() const;
};

struct LetExp : public Expression {
    std::unique_ptr<GroupedDeclarations> decs;
    std::unique_ptr<Expression> body;

    LetExp (GroupedDeclarations *decs, Expression *body, Position pos) : Expression(pos), decs(decs), body(body) {}
    void print() const;
};

struct BreakExp : public Expression {
    BreakExp (Position pos) : Expression(pos) {}
    void print() const;
};

struct ArrayExp : public Expression {
    std::unique_ptr<Symbol> ty;
    std::unique_ptr<Expression> size, init;

    ArrayExp (Symbol *ty, Expression *size, Expression *init, Position pos) : Expression(pos), ty(ty), size(size), init(init) {}
    void print() const;
};

/**
 * Declarations
 * **/
struct Declaration {
    virtual ~Declaration() = default;
    Position pos;
    Declaration(Position pos) : pos(pos) {}
    virtual void print() const = 0;
};

struct VarDec : public Declaration {
    std::unique_ptr<Symbol> id;
    std::unique_ptr<Symbol> type_id;
    std::unique_ptr<Expression> exp;
    bool escape = true;

    VarDec(Symbol *id, Expression *exp, Position pos) : Declaration(pos), id(id), type_id(), exp(exp) {}
    VarDec(Symbol *id, Symbol *type_id, Expression *exp, Position pos) : Declaration(pos), id(id), type_id(type_id), exp(exp) {}

    void print() const;
};

struct TypeDec : public Declaration {
    std::unique_ptr<Symbol> type_id;
    std::unique_ptr<Type> ty;

    TypeDec(Symbol *type_id, Type *ty, Position pos) : Declaration(pos), type_id(type_id), ty(ty) {}
    void print() const;
};

struct FunDec : public Declaration {
    std::unique_ptr<Symbol> id;
    std::unique_ptr<TypeFieldList> tyfields;
    std::unique_ptr<Symbol> type_id;
    std::unique_ptr<Expression> exp;

    FunDec(Symbol *id, TypeFieldList *tyfields, Expression *exp, Position pos) : Declaration(pos), id(id), tyfields(tyfields), type_id(), exp(exp) {}
    FunDec(Symbol *id, TypeFieldList *tyfields, Symbol *type_id, Expression *exp, Position pos) : Declaration(pos), id(id), tyfields(tyfields), type_id(type_id), exp(exp) {}
    void print() const;
};

class GroupedDeclarations : public util::GenericList<DeclarationList> {
public:
    void frontAppendDeclaration(Declaration *dec);
};



};

#endif
