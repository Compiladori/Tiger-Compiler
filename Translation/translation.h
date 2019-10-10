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
    // TODO: Check if these return types are correct, or if they should be pointers of some sort
    trans::AssociatedExpType* transExpression(ast::Expression* exp);
    trans::AssociatedExpType* transDeclaration(ast::Declaration* dec);
    void transVariable(ast::Variable* var);
    trans::ExpType* transType(ast::Type* type);
};


};

#endif
