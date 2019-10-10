#ifndef __ENVIRONMENT_TABLE_H__
#define __ENVIRONMENT_TABLE_H__

#include <unordered_map>
#include <stack>
#include <vector>
#include "../AST/AST.h"
#include "expressionType.h"

namespace trans{


/**
 * Data structures
 * **/
template <class T>
class BindingTable {
    std::unordered_map<ast::Symbol, std::stack<T>, ast::SymbolHasher> table;
public:
    BindingTable() : table() {};
    
    T& operator[](const ast::Symbol& s) const { return table[s]; }
};

struct EnvEntry {
    virtual void print() const = 0;
};

struct VarEntry : public EnvEntry {
    trans::ExpType *ty;

    VarEntry (trans::ExpType *ty) : ty(ty) {}

    void print() const {}
};

struct FunEntry : public EnvEntry {
    std::vector<trans::ExpType*> formals; // TODO: Verify if this is the correct type
    trans::ExpType *result;

    FunEntry (std::vector<trans::ExpType*> formals, trans::ExpType *result) : formals(formals), result(result) {}
    // TODO: Check if complete usage functions are needed (in case formals is not directly built from outside)
    
    void print() const {}
};

typedef BindingTable<trans::ExpType*> TypeEnvironment; // Symbol -> Expression Type
typedef BindingTable<EnvEntry*> ValueEnvironment; // Symbol -> Value (an Environment Entry)



};

#endif
