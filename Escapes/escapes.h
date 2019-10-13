#ifndef __ESCAPES_H__
#define __ESCAPES_H__

/**
 * Escaping variables
 * 
 * Described in Page 140-141 Appel C (2004)
 * */

#include <memory>
#include "../AST/AST.h"
#include "../Translation/environmentTable.h"

namespace esc {


class Escapator {
    trans::BindingTable<trans::EscapeEntry> EscapeEnv;
    int current_depth = 0;
    
    void clear(){
        EscapeEnv.clear();
        current_depth = 0;
    }
    
    auto getEscapeEntry(ast::Symbol s){ return EscapeEnv.getEntry(s); }
    void insertEscapeEntry(ast::Symbol s, std::unique_ptr<trans::EscapeEntry> escape_entry){
        EscapeEnv[s].push(std::move(escape_entry));
    }
    
    void traverseExpression(ast::Expression* exp);
    void traverseDeclaration(ast::Declaration* exp);
    void traverseVariable(ast::Variable* exp);
public:
    Escapator() : current_depth(0) {}
    
    void setEscapes(ast::Expression* exp);
};


};

#endif
