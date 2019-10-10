#include "translation.h"
#include "../AST/AST.h"

#include <cassert>

using namespace trans;

/**
 * Translator
 * **/
// TODO: Check if these return types are correct
AssociatedExpType* Translator::transExpression(ast::Expression* exp){
    // TODO: Complete all the cases
    if(auto var_exp = dynamic_cast<ast::VarExp*>(exp)){
        // TODO: ...
    }
    
    if(auto unit_exp = dynamic_cast<ast::UnitExp*>(exp)){
        // TODO: ...
    }
        
    if(auto nil_exp = dynamic_cast<ast::NilExp*>(exp)){
        // TODO: ...
    }
        
    if(auto int_exp = dynamic_cast<ast::IntExp*>(exp)){
        // TODO: ...
    }
        
    if(auto string_exp = dynamic_cast<ast::StringExp*>(exp)){
        // TODO: ...
    }
        
    if(auto call_exp = dynamic_cast<ast::CallExp*>(exp)){
        // TODO: ...
    }
    
    if(auto op_exp = dynamic_cast<ast::OpExp*>(exp)){
        // TODO: ...
    }
        
    if(auto record_exp = dynamic_cast<ast::RecordExp*>(exp)){
        // TODO: ...
    }
            
    if(auto seq_exp = dynamic_cast<ast::SeqExp*>(exp)){
        // TODO: ...
    }
        
    if(auto assign_exp = dynamic_cast<ast::AssignExp*>(exp)){
        // TODO: ...
    }
        
    if(auto if_exp = dynamic_cast<ast::IfExp*>(exp)){
        // TODO: ...
    }
        
    if(auto while_exp = dynamic_cast<ast::WhileExp*>(exp)){
        // TODO: ...
    }
        
    if(auto for_exp = dynamic_cast<ast::ForExp*>(exp)){
        // TODO: ...
    }
        
    if(auto let_exp = dynamic_cast<ast::LetExp*>(exp)){
        // TODO: ...
    }
        
    if(auto break_exp = dynamic_cast<ast::BreakExp*>(exp)){
        // TODO: ...
    }
        
    if(auto array_exp = dynamic_cast<ast::ArrayExp*>(exp)){
        // TODO: ...
    }
    
    // TODO: Replace with custom error reporting
    // It should have matched some clause of the above
    assert(false);
};

AssociatedExpType* Translator::transDeclaration(ast::Declaration* dec){
    // TODO: Complete all the cases
    if(auto var_dec = dynamic_cast<ast::VarDec*>(dec)){
        // TODO: ...
    }
        
    if(auto fun_dec = dynamic_cast<ast::FunDec*>(dec)){
        // TODO: ...
    }
        
    if(auto type_dec = dynamic_cast<ast::TypeDec*>(dec)){
        // TODO: ...
    }
    
    // TODO: Replace with custom error reporting
    // It should have matched some clause of the switch
    assert(false);
};

void Translator::transVariable(ast::Variable* var){
    if(auto simple_var = dynamic_cast<ast::SimpleVar*>(var)){
        // TODO: ...
    }
        
    if(auto field_var = dynamic_cast<ast::FieldVar*>(var)){
        // TODO: ...
    }
        
    if(auto subscript_var = dynamic_cast<ast::SubscriptVar*>(var)){
        // TODO: ...
    }
    
    // TODO: Replace with custom error reporting
    // It should have matched some clause of the switch
    assert(false);
};

ExpType* Translator::transType(ast::Type* type){
    if(auto name_type = dynamic_cast<ast::NameType*>(type)){
        // TODO: ...
    }
        
    if(auto record_type = dynamic_cast<ast::RecordType*>(type)){
        // TODO: ...
    }
        
    if(auto array_type = dynamic_cast<ast::ArrayType*>(type)){
        // TODO: ...
    }
    
    // TODO: Replace with custom error reporting
    // It should have matched some clause of the switch
    assert(false);
};
