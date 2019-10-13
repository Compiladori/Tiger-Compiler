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
    BindingTable<trans::ExpType> TypeEnv;
    BindingTable<trans::EnvEntry> ValueEnv;
    
    std::stack<std::stack<ast::Symbol>> type_insertions, value_insertions;
    
    auto getTypeEntry(const ast::Symbol& s)  { return TypeEnv.getEntry(s); }
    auto getValueEntry(const ast::Symbol& s) { return ValueEnv.getEntry(s); }
    
    void beginScope();
    void endScope();
    
    void insertTypeEntry(ast::Symbol s, trans::ExpType *exp_type);
    void insertValueEntry(ast::Symbol s, trans::EnvEntry *env_entry);
    
    void load_initial_values(){
        // Basic types
        TypeEnv["int"].emplace(std::make_unique<trans::IntExpType>());
        TypeEnv["string"].emplace(std::make_unique<trans::StringExpType>());
        
        // Runtime functions
        // ValueEnv["print"].emplace(std::make_unique< trans::FunEntry ??? >( ??? ))
        // ValueEnv["flush"].emplace(std::make_unique< trans::FunEntry ??? >( ??? ))
        // ValueEnv["getchar"].emplace(std::make_unique< trans::FunEntry ??? >( ??? ))
        // ValueEnv["ord"].emplace(std::make_unique< trans::FunEntry ??? >( ??? ))
        // ValueEnv["chr"].emplace(std::make_unique< trans::FunEntry ??? >( ??? ))
        // ValueEnv["size"].emplace(std::make_unique< trans::FunEntry ??? >( ??? ))
        // ValueEnv["substring"].emplace(std::make_unique< trans::FunEntry ??? >( ??? ))
        // ValueEnv["concat"].emplace(std::make_unique< trans::FunEntry ??? >( ??? ))
        // ValueEnv["not"].emplace(std::make_unique< trans::FunEntry ??? >( ??? ))
        // ValueEnv["exit"].emplace(std::make_unique< trans::FunEntry ??? >( ??? ))
    }
    
    void clear(){
        TypeEnv.clear(), ValueEnv.clear();
        type_insertions = std::stack<std::stack<ast::Symbol>>();
        value_insertions = std::stack<std::stack<ast::Symbol>>();
        
        load_initial_values();
    }
    
    // TODO: Check if these return types are correct
    trans::AssociatedExpType transVariable(ast::Variable *var);
    trans::AssociatedExpType transExpression(ast::Expression *exp);
    void                     transDeclaration(ast::Declaration *dec);
    trans::ExpType*          transType(ast::Type *type);
public:
    Translator() { load_initial_values(); }
    
    trans::AssociatedExpType translate(ast::Expression *exp);
};


};

#endif
