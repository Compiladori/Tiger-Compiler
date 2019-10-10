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

enum EnvEntryKind {VarEEK, FunEEK, NoEEK};

class EnvEntry {
    EnvEntryKind EEK;
public:
    EnvEntry () : EEK(EnvEntryKind::NoEEK) {}
    EnvEntry (EnvEntryKind EEK) : EEK(EEK) {}
    EnvEntryKind getKind(){ return EEK; }

    virtual void print() = 0;
};

class VarEntry : public EnvEntry {
    trans::ExpType *ty;
public:
    VarEntry (trans::ExpType *ty) : EnvEntry(EnvEntryKind::VarEEK), ty(ty) {}

    void print(){}
};

class FunEntry : public EnvEntry {
    std::vector<trans::ExpType*> formals; // TODO: Verify if this is the correct type
    trans::ExpType *result;
public:
    FunEntry (std::vector<trans::ExpType*>& formals, trans::ExpType *result) : EnvEntry(EnvEntryKind::FunEEK), formals(formals), result(result) {}
    // TODO: Complete with usage functions (in case formals is not directly built from outside)
    
    void print(){}
};

typedef BindingTable<trans::ExpType*> TypeEnvironment; // Symbol -> Expression Type
typedef BindingTable<EnvEntry*> ValueEnvironment; // Symbol -> Value (an Environment Entry)



};

#endif
