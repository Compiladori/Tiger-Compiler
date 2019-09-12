#ifndef __AST_H__
#define __AST_H__

#include <string>

namespace ast {



/** Operations **/
typedef enum {Plus, Minus, Times, Divide, Eq, Neq, Lt, Le, Gt, Ge} Operation;



/** Symbols (Identifiers) **/
class Symbol {
    std::string name;
public:
    Symbol(std::string name);
    std::string get_name();
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

typedef std::vector<TypeField*> TypeFields;

class NameType : public Type {
    Symbol type_id;
public:
    NameType (Symbol type_id);
};

class RecordType : public Type {
    TypeFields *tyfields;
public:
    RecordType (TypeFields &tyfields);
};

class ArrayType : public Type {
    Symbol type_id;
public:
    ArrayType (Symbol type_id);
};



/** Expressions **/
class Expression {
public:
    virtual void print();
};





/** Declarations **/
class Declaration {
public:
    virtual void print();
};

class VarDec : public Declaration {
    Symbol id;
    Symbol type_id;
    Expression *exp;
public:
    VarDec(Symbol id, Expression &exp);
    VarDec(Symbol id, Symbol type_id, Expression &exp);
};

class TypeDec : public Declaration {
    Symbol type_id;
    Type *ty;
public:
    TypeDec(Symbol type_id, Type &ty);
};

class FunDec : public Declaration {
    Symbol id;
    TypeFields *tyfields;
    Symbol type_id;
    Expression *exp;
public:
    FunDec(Symbol id, TypeFields &tyfields, Expression &exp);
    FunDec(Symbol id, TypeFields &tyfields, Symbol type_id, Expression &exp);
};



};

#endif
