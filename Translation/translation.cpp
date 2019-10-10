#include "translation.h"

using namespace trans;

/**
 * Translator
 * **/
// TODO: Check if these return types are correct, or if they should be pointers or some sort
AssociatedExpType* Translator::transExpression(ast::Expression* exp);
AssociatedExpType* Translator::transDeclaration(ast::Declaration* dec);
void Translator::transVariable(ast::Variable* var);
ExpType* Translator::transType(ast::Type* type);
