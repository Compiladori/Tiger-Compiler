#ifndef __ESCAPES_H__
#define __ESCAPES_H__

/**
 * Escaping variables
 * 
 * Described in Page 140-141 Appel C (2004)
 * */

#include "../AST/AST.h"
#include "../Translation/environmentTable.h"

namespace esc {


struct EscapeEntry {
    int depth;
    bool *escape;
    
    EscapeEntry(int depth, bool *escape) : depth(depth), escape(escape) {}
};

class Escapator {
    trans::BindingTable<EscapeEntry*> EscapeEnv;
    int current_depth;
    
    void clear(){
        EscapeEnv.clear();
        current_depth = 0;
    }
    
    EscapeEntry* getEscapeEntry(const ast::Symbol& s) {
        try { return EscapeEnv.getEntry(s); }
        catch (std::exception& e) { return nullptr; }
    }
    
    void insertEscapeEntry(ast::Symbol& s, EscapeEntry *escape_entry);
    
    void traverseExpression(ast::Expression *exp);
    void traverseDeclaration(ast::Declaration *exp);
    void traverseVariable(ast::Variable *exp);
public:
    Escapator() : EscapeEnv(), current_depth(0) {}
    
    void setEscapes(ast::Expression *exp);
};


};

#endif
