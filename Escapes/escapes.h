#ifndef __ESCAPES_H__
#define __ESCAPES_H__

/**
 * Escaping variables
 * 
 * Described in Page 140-141 Appel C (2004)
 * */

#include <memory>
#include <stack>
#include "../AST/AST.h"
#include "../Semantic/environmentTable.h"

namespace esc {

/**
 * Table entries
 * **/
struct EscapeEntry {
    int depth;
    bool* escape;
    
    EscapeEntry(int depth, bool* escape) : depth(depth), escape(escape) {}
};


/**
 * Main escaping class
 * **/
class Escapator {
    seman::BindingTable<EscapeEntry> EscapeEnv;
    std::stack<std::stack<ast::Symbol>> escape_insertions;
    int current_depth = 0;
    
    auto getEscapeEntry(ast::Symbol s){ return EscapeEnv.getEntry(s); }
    void insertEscapeEntry(ast::Symbol s, std::unique_ptr<EscapeEntry> escape_entry, bool ignore_scope = false);
    
    void beginScope();
    void endScope();
    
    void clear(){
        EscapeEnv.clear();
        escape_insertions = std::stack<std::stack<ast::Symbol>>();
        current_depth = 0;
    }
    
    void traverseExpression(ast::Expression* exp);
    void traverseDeclarations(ast::DeclarationList* dec_list);
    void traverseVariable(ast::Variable* var);
public:
    Escapator() = default;
    
    void setEscapes(ast::Expression* exp);
};


};

#endif
