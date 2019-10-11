#ifndef __EXPRESSION_TYPE_H__
#define __EXPRESSION_TYPE_H__

#include <string>

namespace trans{


/**
 * Expression Types
 * **/
enum ExpTypeKind { UnitKind, NilKind, IntKind, StringKind, ArrayKind, RecordKind, CustomKind, NoKind };

struct ExpType {
    ExpTypeKind kind;
    
    ExpType() : kind(ExpTypeKind::NoKind) {}
    ExpType(ExpTypeKind kind) : kind(kind) {}
    
    virtual void print() const = 0;
};

struct UnitExpType : public ExpType {
    UnitExpType() : ExpType(ExpTypeKind::UnitKind) {}
    
    void print() const {}
};

struct NilExpType : public ExpType {
    NilExpType() : ExpType(ExpTypeKind::NilKind) {}
    
    void print() const {}
};

struct IntExpType : public ExpType {
    IntExpType() : ExpType(ExpTypeKind::IntKind) {}
    
    void print() const {}
};

struct StringExpType : public ExpType {
    StringExpType() : ExpType(ExpTypeKind::StringKind) {}
    
    void print() const {}
};

struct ArrayExpType : public ExpType {
    ExpType *type;

    ArrayExpType(ExpType *type) : ExpType(ExpTypeKind::ArrayKind), type(type) {}
    
    void print() const {}
};

struct RecordExpType : public ExpType {
    std::string name;
    ExpType *type;
    int index;

    RecordExpType(std::string name, ExpType *type, int index) : ExpType(ExpTypeKind::RecordKind), name(name), type(type), index(index) {}
    
    void print() const {}
};

struct CustomExpType : public ExpType {
    std::string name;

    CustomExpType (std::string name) : ExpType(ExpTypeKind::CustomKind), name(name) {}
    
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
    // TODO: Determine ownership and implement unique_ptr
    TranslatedExp* tr_exp;
    ExpType* exp_type;

    AssociatedExpType (TranslatedExp *tr_exp, ExpType *exp_type) : tr_exp(tr_exp), exp_type(exp_type) {}
};



};

#endif
