#ifndef __TRANSLATION_H__
#define __TRANSLATION_H__

#include "../AST/AST.h"
#include "environmentTable.h"
#include "expressionType.h"

namespace trans{


class Translator {
    trans::TypeEnvironment TEnv;
    trans::ValueEnvironment VEnv;
public:
    Translator() {
        // TODO: Initialize tables with their initial values,
        // such as "int" and "string" basic types or runtime functions
    }
    
    // TODO: Check if these return types are correct
    trans::AssociatedExpType* transExpression(ast::Expression* exp);
    trans::AssociatedExpType* transDeclaration(ast::Declaration* dec);
    void transVariable(ast::Variable* var);
    trans::ExpType* transType(ast::Type* type);
};


};

#endif
