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
    
    TypeEntry (auto type) : type(type) {}
    
    void print() const {}
};

struct ValueEntry {
    virtual ~ValueEntry() {}
    virtual void print() const = 0;
};

struct VarEntry : public ValueEntry {
    std::shared_ptr<trans::ExpType> type;

    VarEntry (auto type) : type(type) {}

    void print() const {}
};

struct FunEntry : public ValueEntry {
    std::vector<std::shared_ptr<trans::ExpType>> formals;
    std::shared_ptr<trans::ExpType> result;

    FunEntry (auto result) : formals(), result(result) {}
    FunEntry (auto formals, auto result) : formals(formals), result(result) {}
    
    void print() const {}
};

/**
 * Main translating class
 * **/
class Translator {
    BindingTable<TypeEntry> TypeEnv;
    BindingTable<ValueEntry> ValueEnv;
    
    std::stack<std::stack<ast::Symbol>> type_insertions, value_insertions;
    
    auto getTypeEntry(const ast::Symbol& s)  { return TypeEnv.getEntry(s); }
    auto getValueEntry(const ast::Symbol& s) { return ValueEnv.getEntry(s); }
    
    void insertTypeEntry(ast::Symbol s, std::unique_ptr<TypeEntry> type_entry, bool ignore_scope = false);
    void insertValueEntry(ast::Symbol s, std::unique_ptr<ValueEntry> value_entry, bool ignore_scope = false);
    
    void beginScope();
    void endScope();
    
    void load_initial_values();
    
    void clear(){
        TypeEnv.clear(), ValueEnv.clear();
        type_insertions = std::stack<std::stack<ast::Symbol>>();
        value_insertions = std::stack<std::stack<ast::Symbol>>();
        
        load_initial_values();
    }
    
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
