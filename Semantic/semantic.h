#ifndef __SEMANTIC_H__
#define __SEMANTIC_H__

/**
 * Generation of intermediate code and semantic check
 * 
 * Described in Chapter 5 Appel C (2004)
 * **/

#include <cassert>
#include <memory>
#include "../AST/AST.h"
#include "environmentTable.h"
#include "expressionType.h"

namespace seman {
    
/** 
 * Table entries
 * **/
struct TypeEntry {
    std::shared_ptr<ExpType> type;
    
    TypeEntry (std::shared_ptr<ExpType> type) : type(type) {}
    
    void print() const {}
};

struct ValueEntry {
    virtual ~ValueEntry() {}
    virtual void print() const = 0;
};

struct VarEntry : public ValueEntry {
    std::shared_ptr<ExpType> type;

    VarEntry (std::shared_ptr<ExpType> type) : type(type) {}

    void print() const {}
};

struct FunEntry : public ValueEntry {
    std::vector<std::shared_ptr<ExpType>> formals;
    std::shared_ptr<ExpType> result;

    FunEntry (std::shared_ptr<ExpType> result) : formals(), result(result) {}
    FunEntry (std::vector<std::shared_ptr<ExpType>>&  formals, std::shared_ptr<ExpType> result) : formals(formals), result(result) {}
    FunEntry (std::vector<std::shared_ptr<ExpType>>&& formals, std::shared_ptr<ExpType> result) : formals(formals), result(result) {}
    
    void print() const {}
};

/**
 * Main semantic checking class
 * **/
class SemanticChecker {
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
    
    seman::AssociatedExpType transVariable(ast::Variable* var);
    seman::AssociatedExpType transExpression(ast::Expression* exp);
    void                     transDeclarations(ast::DeclarationList* dec_list);
    std::shared_ptr<ExpType> transType(ast::Type* type);
public:
    SemanticChecker() { load_initial_values(); }
    
    seman::AssociatedExpType translate(ast::Expression* exp);
};


};

#endif
