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
    std::unordered_map<ast::Symbol, std::stack<std::unique_ptr<T>>, ast::SymbolHasher> table;
public:
    BindingTable() {};
    
    auto clear()                           { table.clear(); }
    auto empty()                     const { return table.empty(); }
    auto size()                      const { return table.size(); }
    auto count(const ast::Symbol& s) const { return table.count(s); }
    
    T* getEntry(const ast::Symbol& s){
        if(table.count(s) and table[s].size()){
            if(table[s].empty()){
                // Error, trying to access non-existing entry
                return nullptr;
            }
            return table[s].top().get();
        }
        // Error, trying to access non-existing symbol in the table
        return nullptr;
    }
    
    auto& operator[](const ast::Symbol& s){ return table[s]; }
    auto& operator[](const char* s)       { return table[ast::Symbol(s)]; }
};

struct EnvEntry {
    virtual void print() const = 0;
};

struct VarEntry : public EnvEntry {
    trans::ExpType *type;

    VarEntry (trans::ExpType *type) : type(type) {}

    void print() const {}
};

struct FunEntry : public EnvEntry {
    std::vector<trans::ExpType*> formals; // TODO: Verify if this is the correct type
    trans::ExpType *result;

    FunEntry (std::vector<trans::ExpType*> formals, trans::ExpType *result) : formals(formals), result(result) {}
    // TODO: Check if complete usage functions are needed (in case formals is not directly built from outside)
    
    void print() const {}
};




};

#endif
