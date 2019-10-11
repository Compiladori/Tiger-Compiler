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
    trans::AssociatedExpType transVariable(ast::Variable* var);
    trans::AssociatedExpType transExpression(ast::Expression* exp);
    void                     transDeclaration(ast::Declaration* dec);
    trans::ExpType*          transType(ast::Type* type);
};


};

#endif
