#include <cassert>
#include "escapes.h"

using namespace esc;

/**
 * Escapator
 * **/
void Escapator::setEscapes(ast::Expression *exp){
    traverseExpression(exp);
}

void Escapator::traverseExpression(ast::Expression *exp){
    // TODO: Complete all the cases
    if(auto var_exp = dynamic_cast<ast::VarExp*>(exp)){
        // TODO: ...
        return;
    }
    
    if(auto unit_exp = dynamic_cast<ast::UnitExp*>(exp)){
        // TODO: ...
        return;
    }
        
    if(auto nil_exp = dynamic_cast<ast::NilExp*>(exp)){
        // TODO: ...
        return;
    }
        
    if(auto int_exp = dynamic_cast<ast::IntExp*>(exp)){
        // TODO: ...
        return;
    }
        
    if(auto string_exp = dynamic_cast<ast::StringExp*>(exp)){
        // TODO: ...
        return;
    }
        
    if(auto call_exp = dynamic_cast<ast::CallExp*>(exp)){
        // TODO: ...
        return;
    }
    
    if(auto op_exp = dynamic_cast<ast::OpExp*>(exp)){
        // TODO: ...
        return;
    }
        
    if(auto record_exp = dynamic_cast<ast::RecordExp*>(exp)){
        // TODO: ...
        return;
    }
            
    if(auto seq_exp = dynamic_cast<ast::SeqExp*>(exp)){
        // TODO: ...
        return;
    }
        
    if(auto assign_exp = dynamic_cast<ast::AssignExp*>(exp)){
        // TODO: ...
        return;
    }
        
    if(auto if_exp = dynamic_cast<ast::IfExp*>(exp)){
        // TODO: ...
        return;
    }
        
    if(auto while_exp = dynamic_cast<ast::WhileExp*>(exp)){
        // TODO: ...
        return;
    }
        
    if(auto for_exp = dynamic_cast<ast::ForExp*>(exp)){
        // TODO: ...
        return;
    }
        
    if(auto let_exp = dynamic_cast<ast::LetExp*>(exp)){
        // TODO: ...
        return;
    }
        
    if(auto break_exp = dynamic_cast<ast::BreakExp*>(exp)){
        // TODO: ...
        return;
    }
        
    if(auto array_exp = dynamic_cast<ast::ArrayExp*>(exp)){
        // TODO: ...
        return;
    }
    
    // Error, it should have matched some clause
    assert(false);
}

void Escapator::traverseDeclaration(ast::Declaration *dec){
    // TODO: Complete all the cases
    if(auto var_dec = dynamic_cast<ast::VarDec*>(dec)){
        // TODO: ...
        return;
    }
    
    if(auto fun_dec = dynamic_cast<ast::FunDec*>(dec)){
        // TODO: ...
        return;
    }
    
    if(auto type_dec = dynamic_cast<ast::TypeDec*>(dec)){
        // TODO: ...
        return;
    }
    
    // Error, it should have matched some clause
    assert(false);
}

void Escapator::traverseVariable(ast::Variable *var){
    // TODO: Complete all the cases
    if(auto simple_var = dynamic_cast<ast::SimpleVar*>(var)){
        // TODO: ...
        return;
    }
        
    if(auto field_var = dynamic_cast<ast::FieldVar*>(var)){
        // TODO: ...
        return;
    }
        
    if(auto subscript_var = dynamic_cast<ast::SubscriptVar*>(var)){
        // TODO: ...
        return;
    }
    
    // Error, it should have matched some clause
    assert(false);
}
