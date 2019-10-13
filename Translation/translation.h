#ifndef __TRANSLATION_H__
#define __TRANSLATION_H__

/**
 * Translation and semantic check
 * 
 * Described in Chapter 5 Appel C (2004)
 * **/

#include <cassert>
#include "../AST/AST.h"
#include "environmentTable.h"
#include "expressionType.h"

namespace trans{

// TODO: Replace assert() with custom error reporting
class Translator {
    BindingTable<trans::ExpType*> TypeEnv;
    BindingTable<trans::EnvEntry*> ValueEnv;
    
    std::stack<std::stack<ast::Symbol>> type_insertions, value_insertions;
    
    void clear(){
        TypeEnv.clear(), ValueEnv.clear();
        type_insertions = std::stack<std::stack<ast::Symbol>>();
        value_insertions = std::stack<std::stack<ast::Symbol>>();
    }
    
    trans::ExpType* getTypeEntry(const ast::Symbol& s) {
        try { return TypeEnv.getEntry(s); }
        catch (std::exception& e) { return nullptr; }
    }
    trans::EnvEntry* getValueEntry(const ast::Symbol& s) {
        try { return ValueEnv.getEntry(s); }
        catch (std::exception& e) { return nullptr; }
    }
    
    void beginScope();
    void endScope();
    
    void insertTypeEntry(ast::Symbol& s, trans::ExpType *exp_type);
    void insertValueEntry(ast::Symbol& s, trans::EnvEntry *env_entry);
    
    // TODO: Check if these return types are correct
    trans::AssociatedExpType transVariable(ast::Variable *var);
    trans::AssociatedExpType transExpression(ast::Expression *exp);
    void                     transDeclaration(ast::Declaration *dec);
    trans::ExpType*          transType(ast::Type *type);
public:
    Translator() {
        // TODO: Initialize tables with their initial values,
        // such as "int" and "string" basic types or runtime functions
    }
    
    trans::AssociatedExpType translate(ast::Expression *exp);
};


};

#endif
