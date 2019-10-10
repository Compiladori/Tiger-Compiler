#ifndef __EXPRESSION_TYPE_H__
#define __EXPRESSION_TYPE_H__

#include <string>

namespace trans{


/**
 * Expression Types
 * **/
struct ExpType {
    virtual void print() const = 0;
};

struct UnitExpType : public ExpType {
    UnitExpType(){}
    
    void print() const {}
};

struct NilExpType : public ExpType {
    NilExpType(){}
    
    void print() const {}
};

struct IntExpType : public ExpType {
    IntExpType(){}
    
    void print() const {}
};

struct StringExpType : public ExpType {
    StringExpType(){}
    
    void print() const {}
};

struct ArrayExpType : public ExpType {
    ExpType *type;

    ArrayExpType(ExpType *type) : type(type) {}
    
    void print() const {}
};

struct RecordExpType : public ExpType {
    std::string name;
    ExpType *type;
    int index;

    RecordExpType(std::string name, ExpType *type, int index) : name(name), type(type), index(index) {}
    
    void print() const {}
};

struct CustomExpType : public ExpType {
    std::string name;

    CustomExpType (std::string name) : name(name) {}
    
    void print() const {}
};

/**
 * Translated Expression associated with an Expression Type
 * **/
struct TranslatedExp {
    // TODO: Incomplete dummy class for translated expressions

    TranslatedExp() {}
};

struct AssociatedExpType {
    TranslatedExp *tr_exp;
    ExpType *exp_type;

    AssociatedExpType (TranslatedExp *tr_exp, ExpType *exp_type) : tr_exp(tr_exp), exp_type(exp_type) {}
};



};

#endif
