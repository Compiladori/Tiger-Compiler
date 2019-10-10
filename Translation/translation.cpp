#include "translation.h"
#include "../AST/AST.h"

#include <cassert>

using namespace trans;

/**
 * Translator
 * **/
// TODO: Check if these return types are correct
AssociatedExpType* Translator::transExpression(ast::Expression* exp){
    switch(exp->getKind()){
        // TODO: Complete all the cases
        case ast::VarEK: {
            // TODO: ...
            break;
        }
        
        case ast::UnitEK: {
            // TODO: ...
            break;
        }
        
        case ast::NilEK: {
            // TODO: ...
            break;
        }
        
        case ast::IntEK: {
            // TODO: ...
            break;
        }
        
        case ast::StringEK: {
            // TODO: ...
            break;
        }
        
        case ast::CallEK: {
            // TODO: ...
            break;
        }
        
        case ast::OpEK: {
            // auto op_exp = static_cast<ast::OpExp*>(exp);
            // auto left = transExpression(op_exp->getLeft());
            // auto right = transExpression(op_exp->getRight());
            
            // TODO: ...
            break;
        }
        
        case ast::RecordEK: {
            // TODO: ...
            break;
        }
            
        case ast::SeqEK: {
            // TODO: ...
            break;
        }
        
        case ast::AssignEK: {
            // TODO: ...
            break;
        }
        
        case ast::IfEK: {
            // TODO: ...
            break;
        }
        
        case ast::WhileEK: {
            // TODO: ...
            break;
        }
        
        case ast::ForEK: {
            // TODO: ...
            break;
        }
        
        case ast::LetEK: {
            // TODO: ...
            break;
        }
        
        case ast::BreakEK: {
            // TODO: ...
            break;
        }
        
        case ast::ArrayEK: {
            // TODO: ...
            break;
        }
        
        case ast::NoEK: {
            // Error, expression should be of some kind
            // TODO: ...
            break;
        }
    }
    
    // TODO: Replace with custom error reporting
    // It should have matched some clause of the switch
    assert(false);
};

AssociatedExpType* Translator::transDeclaration(ast::Declaration* dec){
    switch(dec->getKind()){
        // TODO: Complete all the cases
        case ast::VarDK: {
            // TODO: ...
            break;
        }
        
        case ast::FunDK: {
            // TODO: ...
            break;
        }
        
        case ast::TypeDK: {
            // TODO: ...
            break;
        }
        
        case ast::NoDK: {
            // Error, declaration should be of some kind
            // TODO: ...
            break;
        }
    }
    
    // TODO: Replace with custom error reporting
    // It should have matched some clause of the switch
    assert(false);
};

void Translator::transVariable(ast::Variable* var){
    switch(var->getKind()){
        // TODO: Complete all the cases
        case ast::SimpleVK: {
            // TODO: ...
            break;
        }
        
        case ast::FieldVK: {
            // TODO: ...
            break;
        }
        
        case ast::SubscriptVK: {
            // TODO: ...
            break;
        }
        
        case ast::NoVK: {
            // Error, variable should be of some kind
            // TODO: ...
            break;
        }
    }
    
    // TODO: Replace with custom error reporting
    // It should have matched some clause of the switch
    assert(false);
};

ExpType* Translator::transType(ast::Type* type){
    switch(type->getKind()){
        // TODO: Complete all the cases
        case ast::NameTK: {
            // TODO: ...
            break;
        }
        
        case ast::RecordTK: {
            // TODO: ...
            break;
        }
        
        case ast::ArrayTK: {
            // TODO: ...
            break;
        }
        
        case ast::NoTK: {
            // Error, type should be of some kind
            // TODO: ...
            break;
        }
    }
    
    // TODO: Replace with custom error reporting
    // It should have matched some clause of the switch
    assert(false);
};
