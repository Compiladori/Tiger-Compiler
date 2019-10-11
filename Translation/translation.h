#ifndef __TRANSLATION_H__
#define __TRANSLATION_H__

#include <cassert>
#include "../AST/AST.h"
#include "environmentTable.h"
#include "expressionType.h"

namespace trans{

// TODO: Replace assert() with custom error reporting
class Translator {
    BindingTable<trans::ExpType> TEnv;
    BindingTable<trans::EnvEntry> VEnv;
    
    std::stack<std::stack<ast::Symbol*>> tenv_insertions, venv_insertions;
public:
    Translator() {
        // TODO: Initialize tables with their initial values,
        // such as "int" and "string" basic types or runtime functions
    }
    
    trans::ExpType* getTypeEntry(const ast::Symbol& s) {
        return TEnv.getEntry(s);
    }
    trans::EnvEntry* getValueEntry(const ast::Symbol& s) { 
        return VEnv.getEntry(s);
    }
    
    void beginScope(){
        // Create a new scope without any initial insertions
        tenv_insertions.push(std::stack<ast::Symbol*>());
        venv_insertions.push(std::stack<ast::Symbol*>());
    }
    void endScope(){
        if(tenv_insertions.empty() or venv_insertions.empty()){
            // Error, there is no scope to end
            assert(false);
        }
        
        // Remove all registered scoped insertions
        while(not tenv_insertions.top().empty()){
            auto s = tenv_insertions.top().top();
            tenv_insertions.top().pop();
            TEnv[*s].pop();
        }
        tenv_insertions.pop();
        
        while(not venv_insertions.top().empty()){
            auto s = venv_insertions.top().top();
            venv_insertions.top().pop();
            VEnv[*s].pop();
        }
        venv_insertions.pop();
    }
    
    void insertTypeEntry(ast::Symbol& s, trans::ExpType *exp_type) {
        if(tenv_insertions.empty()){
            // Error, no scope was initialized
            assert(false);
        }
        tenv_insertions.top().push(&s);
        TEnv[s].push(exp_type);
    }
    void insertValueEntry(ast::Symbol& s, trans::EnvEntry *env_entry) {
        if(venv_insertions.empty()){
            // Error, no scope was initialized
            assert(false);
        }
        venv_insertions.top().push(&s);
        VEnv[s].push(env_entry);
    }
    
    // TODO: Check if these return types are correct
    trans::AssociatedExpType transVariable(ast::Variable* var);
    trans::AssociatedExpType transExpression(ast::Expression* exp);
    void                     transDeclaration(ast::Declaration* dec);
    trans::ExpType*          transType(ast::Type* type);
};


};

#endif
