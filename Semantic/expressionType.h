#ifndef __EXPRESSION_TYPE_H__
#define __EXPRESSION_TYPE_H__

#include <string>
#include <vector>
#include <memory>
#include <cassert>
#include "../Translation/translatedExpression.h"

namespace seman {


/**
 * Expression Types
 * **/
// TODO: Replace assert(false) with custom errors
enum ExpTypeKind { UnitKind = 0, NilKind, IntKind, StringKind, ArrayKind, RecordKind, NoKind };


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

/** Kinds **/
static std::string kind_name[] = { [ExpTypeKind::UnitKind] = "unit",
                                   [ExpTypeKind::NilKind] = "nil",
                                   [ExpTypeKind::IntKind] = "int",
                                   [ExpTypeKind::StringKind] = "string",
                                   [ExpTypeKind::ArrayKind] = "array",
                                   [ExpTypeKind::RecordKind] = "record",
                                   [ExpTypeKind::NoKind] = "no type" };

static std::ostream& operator<<(std::ostream& os, const ExpType& k){ return os << kind_name[k.kind]; }

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


class ArrayExpType : public ExpType {
    bool uses_weak;
    std::weak_ptr<ExpType> weak_type;
    std::shared_ptr<ExpType> shared_type;

public:
    ArrayExpType()                              : ExpType(ExpTypeKind::ArrayKind) {}
    ArrayExpType(std::shared_ptr<ExpType> type) : ExpType(ExpTypeKind::ArrayKind) { updateType(type); }

    std::shared_ptr<ExpType> getType() const {
        if(uses_weak) return weak_type.lock();
        else          return shared_type;
    }

    void updateType(std::shared_ptr<ExpType> type){
        uses_weak = type.get() == this;

        if(uses_weak) weak_type   = type, shared_type.reset();
        else          shared_type = type, weak_type.reset();
    }

    virtual bool operator==(const ExpType& exp_type) const override {
        if(exp_type.kind == ExpTypeKind::NilKind)
            return true;

        if(this->kind != exp_type.kind)
            return false;

        return this == &exp_type;
    }

    void print() const override {}
};

class RecordExpTypeField {
    bool uses_weak;
    std::weak_ptr<ExpType> weak_type;
    std::shared_ptr<ExpType> shared_type;

public:
    std::string name;

    std::shared_ptr<ExpType> getType() const {
        if(uses_weak) return weak_type.lock();
        else          return shared_type;
    }

    RecordExpTypeField(std::string& name, std::shared_ptr<ExpType> type, bool uses_weak) : uses_weak(uses_weak), name(name) {
        if(uses_weak) weak_type   = type;
        else          shared_type = type;
    }
};

struct RecordExpType : public ExpType {
    std::vector<RecordExpTypeField> fields;

    RecordExpType()                                         : ExpType(ExpTypeKind::RecordKind) {}
    RecordExpType(std::vector<RecordExpTypeField>&  fields) : ExpType(ExpTypeKind::RecordKind), fields(fields) {}
    RecordExpType(std::vector<RecordExpTypeField>&& fields) : ExpType(ExpTypeKind::RecordKind), fields(fields) {}

    void pushField(std::string& name, std::shared_ptr<ExpType> type){
        fields.push_back(RecordExpTypeField(name, type, type.get() == this));
    }

    void updateField(std::size_t index, std::shared_ptr<ExpType> type){
        if(index < 0 or index >= fields.size()){
            // Internal error, invalid fields index
            assert(false);
        }

        fields[index] = RecordExpTypeField(fields[index].name, type, type.get() == this);
    }

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

struct AssociatedExpType {
    std::unique_ptr<trans::TranslatedExp> tr_exp;
    std::shared_ptr<ExpType> exp_type;

    AssociatedExpType (std::unique_ptr<trans::TranslatedExp> tr_exp, std::shared_ptr<ExpType> exp_type) : tr_exp(move(tr_exp)), exp_type(exp_type) {}
};



};

#endif
