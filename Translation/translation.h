#ifndef __TRANSLATION_H__
#define __TRANSLATION_H__

/**
 * Translation and semantic check
 * 
 * Described in Chapter 5 Appel C (2004)
 * **/

#include <cassert>
#include <memory>
#include "../AST/AST.h"
#include "environmentTable.h"
#include "expressionType.h"

namespace trans{

// TODO: Replace assert() with custom error reporting
class Translator {
    BindingTable<trans::TypeEntry> TypeEnv;
    BindingTable<trans::ValueEntry> ValueEnv;
    
    std::stack<std::stack<ast::Symbol>> type_insertions, value_insertions;
    
    auto getTypeEntry(const ast::Symbol& s)  { return TypeEnv.getEntry(s); }
    auto getValueEntry(const ast::Symbol& s) { return ValueEnv.getEntry(s); }
    
    void beginScope();
    void endScope();
    
    void insertTypeEntry(ast::Symbol s, std::shared_ptr<trans::ExpType> type_entry, bool ignore_scope = false);
    void insertValueEntry(ast::Symbol s, std::unique_ptr<trans::ValueEntry> value_entry, bool ignore_scope = false);
    
    void load_initial_values(){
        // Basic types
        insertTypeEntry("int",    std::make_shared<trans::IntExpType>(), true);
        insertTypeEntry("string", std::make_shared<trans::StringExpType>(), true);
        
        // Runtime functions
        // insertValueEntry("print",     std::make_unique<trans::FunEntry>(???), true);
        // insertValueEntry("flush",     std::make_unique<trans::FunEntry>(???), true);
        // insertValueEntry("getchar",   std::make_unique<trans::FunEntry>(???), true);
        // insertValueEntry("ord",       std::make_unique<trans::FunEntry>(???), true);
        // insertValueEntry("chr",       std::make_unique<trans::FunEntry>(???), true);
        // insertValueEntry("size",      std::make_unique<trans::FunEntry>(???), true);
        // insertValueEntry("substring", std::make_unique<trans::FunEntry>(???), true);
        // insertValueEntry("concat",    std::make_unique<trans::FunEntry>(???), true);
        // insertValueEntry("not",       std::make_unique<trans::FunEntry>(???), true);
        // insertValueEntry("exit",      std::make_unique<trans::FunEntry>(???), true);
    }
    
    void clear(){
        TypeEnv.clear(), ValueEnv.clear();
        type_insertions = std::stack<std::stack<ast::Symbol>>();
        value_insertions = std::stack<std::stack<ast::Symbol>>();
        
        load_initial_values();
    }
    
    // TODO: Check if these return types are correct
    trans::AssociatedExpType transVariable(ast::Variable* var);
    trans::AssociatedExpType transExpression(ast::Expression* exp);
    void                     transDeclaration(ast::Declaration* dec);
    std::shared_ptr<ExpType> transType(ast::Type* type);
public:
    Translator() { load_initial_values(); }
    
    trans::AssociatedExpType translate(ast::Expression* exp);
};


};

#endif
