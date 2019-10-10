#ifndef __EXPRESSION_TYPE_H__
#define __EXPRESSION_TYPE_H__

#include <string>

namespace trans{


/**
 * Expression Types
 * **/
class ExpType {
public:
    virtual void print() = 0;
};

class UnitExpType : public ExpType {
public:
    UnitExpType(){}
    
    void print(){}
};

class NilExpType : public ExpType {
public:
    NilExpType(){}
    
    void print(){}
};

class IntExpType : public ExpType {
public:
    IntExpType(){}
    
    void print(){}
};

class StringExpType : public ExpType {
public:
    StringExpType(){}
    
    void print(){}
};

class ArrayExpType : public ExpType {
    ExpType *type;
public:
    ArrayExpType(ExpType *type) : type(type) {}
    
    void print(){}
};

class RecordExpType : public ExpType {
    std::string name;
    ExpType *type;
    int index;
public:
    RecordExpType(std::string name, ExpType *type, int index) : name(name), type(type), index(index) {}
    
    void print(){}
};

class CustomExpType : public ExpType {
    std::string name;
public:
    CustomExpType(std::string name) : name(name) {}
    
    void print(){}
};

/**
 * Expression associated with an Expression Type
 * **/
class AssociatedExpType {
    ast::Expression *exp;
    ExpType *exp_type;
public:
    AssociatedExpType(ast::Expression *exp, ExpType *exp_type) : exp(exp), exp_type(exp_type) {}
};



};

#endif
