#ifndef __ENVIRONMENT_TABLE_H__
#define __ENVIRONMENT_TABLE_H__

#include <unordered_map>
#include <memory>
#include <stack>
#include <vector>
#include "../AST/AST.h"
#include "../Utility/utility.h"
#include "expressionType.h"

namespace seman {


/**
 * Binding table data structure
 * 
 * Maps Symbols to a stack of unique_ptr<T> entries that will be used to keep track of scopes
 * **/

template <class T>
class BindingTable {
    std::unordered_map<ast::Symbol, std::stack<std::unique_ptr<T>>, ast::SymbolHasher> table;
public:
    BindingTable() = default;
    
    auto clear()                           { table.clear(); }
    auto empty()                     const { return table.empty(); }
    auto size()                      const { return table.size(); }
    auto count(const ast::Symbol& s) const { return table.count(s); }
    
    auto& operator[](const ast::Symbol& s){ return table[s]; }
    
    T* getEntry(const ast::Symbol& s){
        if(table.count(s)){
            if(table[s].empty()){
                // Error, trying to access non-existing entry
                return nullptr;
            }
            return table[s].top().get();
        }
        // Error, trying to access non-existing symbol in the table
        return nullptr;
    }    
};



};

#endif
