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
    
/** 
 * Table entries
 * **/
struct TypeEntry {
    std::shared_ptr<trans::ExpType> type;
    
    TypeEntry (std::shared_ptr<trans::ExpType> type) : type(type) {}
    
    void print() const {}
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

/**
 * Main translating class
 * **/

// TODO: Replace assert() with custom error reporting
class Translator {
    BindingTable<TypeEntry> TypeEnv;
    BindingTable<ValueEntry> ValueEnv;
    
    std::stack<std::stack<ast::Symbol>> type_insertions, value_insertions;
    
    auto getTypeEntry(const ast::Symbol& s)  { return TypeEnv.getEntry(s); }
    auto getValueEntry(const ast::Symbol& s) { return ValueEnv.getEntry(s); }
    
    void insertTypeEntry(ast::Symbol s, std::shared_ptr<trans::ExpType> type_entry, bool ignore_scope = false);
    void insertValueEntry(ast::Symbol s, std::unique_ptr<ValueEntry> value_entry, bool ignore_scope = false);
    
    void beginScope();
    void endScope();    
    
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
    void                     transDeclarations(ast::DeclarationList* dec_list);
    std::shared_ptr<ExpType> transType(ast::Type* type);
public:
    Translator() { load_initial_values(); }
    
    trans::AssociatedExpType translate(ast::Expression* exp);
};


};

#endif
