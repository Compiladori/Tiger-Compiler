#ifndef __EXPRESSION_TYPE_H__
#define __EXPRESSION_TYPE_H__

#include <string>
#include <vector>
#include <memory>

namespace trans{


/**
 * Expression Types
 * **/
enum ExpTypeKind { UnitKind = 0, NilKind, IntKind, StringKind, ArrayKind, RecordKind, CustomKind, NoKind };

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
    
    void print() const override {}
};

struct NilExpType : public ExpType {
    NilExpType() : ExpType(ExpTypeKind::NilKind) {}
    
    virtual bool operator==(const ExpType& exp_type) const override {
        if(exp_type.kind == ExpTypeKind::RecordKind)
            return true;
        
        if(exp_type.kind == ExpTypeKind::ArrayKind)
            return true;
        
        return this->kind == exp_type.kind;
    }
    
    void print() const override {}
};

struct IntExpType : public ExpType {
    IntExpType() : ExpType(ExpTypeKind::IntKind) {}

    void print() const override {}
};

struct StringExpType : public ExpType {
    StringExpType() : ExpType(ExpTypeKind::StringKind) {}

    void print() const override {}
};

struct ArrayExpType : public ExpType {
    std::shared_ptr<ExpType> type; // TODO: Memory leak, not using weak references to itself

    ArrayExpType()                              : ExpType(ExpTypeKind::ArrayKind), type() {}
    ArrayExpType(std::shared_ptr<ExpType> type) : ExpType(ExpTypeKind::ArrayKind), type(type) {}
    
    virtual bool operator==(const ExpType& exp_type) const override {
        if(exp_type.kind == ExpTypeKind::NilKind)
            return true;
        
        if(this->kind != exp_type.kind)
            return false;
        
        return this == &exp_type;
    }
    
    void print() const override {}
};

struct RecordExpTypeField {
    std::string name;
    std::shared_ptr<ExpType> type; // TODO: Memory leak, not using weak references to itself
    
    RecordExpTypeField(std::string name)                                : name(name), type() {}
    RecordExpTypeField(std::string name, std::shared_ptr<ExpType> type) : name(name), type(type) {}
}; 

struct RecordExpType : public ExpType {
    std::vector<RecordExpTypeField> fields;

    RecordExpType()                                         : ExpType(ExpTypeKind::RecordKind) {}
    RecordExpType(std::vector<RecordExpTypeField>&  fields) : ExpType(ExpTypeKind::RecordKind), fields(fields) {}
    RecordExpType(std::vector<RecordExpTypeField>&& fields) : ExpType(ExpTypeKind::RecordKind), fields(fields) {}
    
    virtual bool operator==(const ExpType& exp_type) const override {
        if(exp_type.kind == ExpTypeKind::NilKind)
            return true;
        
        if(this->kind != exp_type.kind)
            return false;
        
        return this == &exp_type;
    }
    
    void print() const override {}
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

    AssociatedExpType (std::shared_ptr<TranslatedExp> tr_exp, std::shared_ptr<ExpType> exp_type) : tr_exp(tr_exp), exp_type(exp_type) {}
};



};

#endif
