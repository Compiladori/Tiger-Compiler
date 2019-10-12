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
    bool* escape; // TODO: Check if a pointer is actually necessary
};

class Escapator {
    trans::BindingTable<EscapeEntry> EscapeEnv;
    int current_depth = 0;
    
    void traverseExpression(ast::Expression *exp);
    void traverseDeclaration(ast::Declaration *exp);
    void traverseVariable(ast::Variable *exp);
public:
    Escapator() {}
    
    void setEscapes(ast::Expression *exp);
};


};

#endif
