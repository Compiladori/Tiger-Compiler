#ifndef __EXPRESSION_TYPE_H__
#define __EXPRESSION_TYPE_H__

#include <string>
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

    ArrayExpType(std::shared_ptr<ExpType> type) : ExpType(ExpTypeKind::ArrayKind), type(std::move(type)) {}
    
    bool operator==(const ExpType& exp_type) const {
        if(this->kind != exp_type.kind)
            return false;
        
        return *this->type == *static_cast<const ArrayExpType*>(&exp_type)->type;
    }
    
    void print() const {}
};

struct RecordExpType : public ExpType {
    std::string name;
    std::shared_ptr<ExpType> type;
    int index;

    RecordExpType(std::string name, std::shared_ptr<ExpType> type, int index) : ExpType(ExpTypeKind::RecordKind), name(name), type(type), index(index) {}
    
    bool operator==(const ExpType& exp_type) const {
        if(this->kind != exp_type.kind)
            return false;
        
        return this == &exp_type;
    }
    
    void print() const {}
};

struct CustomExpType : public ExpType {
    std::string name;

    CustomExpType (std::string name) : ExpType(ExpTypeKind::CustomKind), name(name) {}
    
    bool operator==(const ExpType& exp_type) const {
        if(this->kind != exp_type.kind)
            return false;
        
        return this->name == static_cast<const CustomExpType*>(&exp_type)->name;
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

    AssociatedExpType (std::shared_ptr<TranslatedExp> tr_exp, std::shared_ptr<ExpType> exp_type) : tr_exp(tr_exp), exp_type(exp_type) {}
};



};

#endif
