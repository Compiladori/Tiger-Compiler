#ifndef __ENVIRONMENT_TABLE_H__
#define __ENVIRONMENT_TABLE_H__

#include <unordered_map>
#include <memory>
#include <stack>
#include <vector>
#include "../AST/AST.h"
#include "../Utility/utility.h"
#include "expressionType.h"

namespace trans{


/**
 * Binding table data structure
 * 
 * Maps Symbols to a stack of unique_ptr<T> entries that will be used to keep track of scopes
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
};

/** 
 * Table entries
 * **/

struct TypeEntry {
    std::shared_ptr<trans::ExpType> type;
    
    TypeEntry (std::shared_ptr<trans::ExpType> type) : type(type) {}
    
    void print() const {}
};

struct EscapeEntry {
    int depth;
    bool* escape;
    
    EscapeEntry(int depth, bool* escape) : depth(depth), escape(escape) {}
};

struct ValueEntry {
    virtual void print() const = 0;
};

struct VarEntry : public ValueEntry {
    std::shared_ptr<trans::ExpType> type;

    VarEntry (std::shared_ptr<trans::ExpType> type) : type(type) {}

    void print() const {}
};

struct FunEntry : public ValueEntry {
    std::vector<std::shared_ptr<trans::ExpType>> formals; // TODO: Verify if this is the correct type
    std::shared_ptr<trans::ExpType> result;

    FunEntry (auto formals, std::shared_ptr<trans::ExpType> result) : formals(formals), result(result) {}
    
    void print() const {}
};




};

#endif
