#ifndef __EXPRESSION_TYPE_H__
#define __EXPRESSION_TYPE_H__

#include <string>
#include <vector>
#include <memory>

namespace trans{


/**
 * Expression Types
 * **/
enum ExpTypeKind { UnitKind, NilKind, IntKind, StringKind, ArrayKind, RecordKind, CustomKind, NoKind };

struct ExpType {
    ExpTypeKind kind;
    
    ExpType() : kind(ExpTypeKind::NoKind) {}
    ExpType(ExpTypeKind kind) : kind(kind) {}
    
    virtual bool operator==(const ExpType& exp_type) const {
        // By default, two types are the same if their kinds match
        return this->kind == exp_type.kind;
    }
    virtual bool operator!=(const ExpType& exp_type) const {
        return not (*this == exp_type);
    }
    
    virtual ~ExpType() {}
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
    std::shared_ptr<ExpType> type;

    ArrayExpType(auto type) : ExpType(ExpTypeKind::ArrayKind), type(type) {}
    
    bool operator==(const ExpType& exp_type) const {
        if(exp_type.kind == ExpTypeKind::NilKind)
            return true;
        
        if(this->kind != exp_type.kind)
            return false;
        
        return this == &exp_type;
    }
    
    void print() const {}
};

struct RecordExpTypeField {
    std::string name;
    std::shared_ptr<ExpType> type;
    int index;
    
    RecordExpTypeField(auto name, auto type, int index) : name(name), type(type), index(index) {}
}; 

struct RecordExpType : public ExpType {
    std::vector<RecordExpTypeField> fields;

    RecordExpType()            : ExpType(ExpTypeKind::RecordKind) {}
    RecordExpType(auto fields) : ExpType(ExpTypeKind::RecordKind), fields(fields) {}
    
    bool operator==(const ExpType& exp_type) const {
        if(this->kind != exp_type.kind)
            return false;
        
        return this == &exp_type;
    }
    
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
    std::shared_ptr<TranslatedExp> tr_exp;
    std::shared_ptr<ExpType> exp_type;

    AssociatedExpType (auto tr_exp, auto exp_type) : tr_exp(tr_exp), exp_type(exp_type) {}
};



};

#endif
