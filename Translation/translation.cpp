#include "translation.h"
#include "../AST/AST.h"

#include <cassert>

using namespace trans;

/**
 * Translator
 * **/
// TODO: Check if these return types are correct
// TODO: Replace assert() with custom error reporting, including Position()
AssociatedExpType Translator::transVariable(ast::Variable* var){
    // TODO: Complete all the cases
    if(auto simple_var = dynamic_cast<ast::SimpleVar*>(var)){
        auto env_entry = getValueEntry(*simple_var->id);
        if(auto var_entry = dynamic_cast<VarEntry*>(env_entry)){
            // TODO: Fix 'var_entry->type' to the actual type expected (Page 117-118 Appel C)
            return AssociatedExpType(new TranslatedExp, var_entry->type);
        }
        // Error, undefined variable
        assert(false);
    }
        
    if(auto field_var = dynamic_cast<ast::FieldVar*>(var)){
        // TODO: ...
    }
        
    if(auto subscript_var = dynamic_cast<ast::SubscriptVar*>(var)){
        // TODO: ...
    }
    
    // Error, it should have matched some clause of the above
    assert(false);
};

AssociatedExpType Translator::transExpression(ast::Expression* exp){
    // TODO: Complete all the cases
    if(auto var_exp = dynamic_cast<ast::VarExp*>(exp)){
        // TODO: ...
    }
    
    if(auto unit_exp = dynamic_cast<ast::UnitExp*>(exp)){
        return AssociatedExpType(new TranslatedExp, new UnitExpType);
    }
        
    if(auto nil_exp = dynamic_cast<ast::NilExp*>(exp)){
        return AssociatedExpType(new TranslatedExp, new NilExpType);
    }
        
    if(auto int_exp = dynamic_cast<ast::IntExp*>(exp)){
        return AssociatedExpType(new TranslatedExp, new IntExpType);
    }
        
    if(auto string_exp = dynamic_cast<ast::StringExp*>(exp)){
        return AssociatedExpType(new TranslatedExp, new StringExpType);
    }
        
    if(auto call_exp = dynamic_cast<ast::CallExp*>(exp)){
        // TODO: ...
    }
    
    if(auto op_exp = dynamic_cast<ast::OpExp*>(exp)){
        auto oper = op_exp->oper;
        auto result_left = transExpression(op_exp->left.get());
        auto result_right = transExpression(op_exp->right.get());
        
        switch(oper){
            case ast::Plus: {
                if(result_left.exp_type->kind != ExpTypeKind::IntKind){
                    // Error, integer required on the left
                    assert(false);
                }
                if(result_right.exp_type->kind != ExpTypeKind::IntKind){
                    // Error, integer required on the right
                    assert(false);
                }
                return AssociatedExpType(new TranslatedExp, new IntExpType);
            }
            // TODO: Complete the rest of the operators
        }
        
        // Error, the operator should have matched some clause of the switch
        assert(false);
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
        beginScope();
        
        // Augment current scope by processing let declarations:
        // for declist in *let_exp->decs
        //   transDeclaration(declist)
        
        auto result = transExpression(let_exp->body.get());
        
        endScope();
        
        return result;
    }
        
    if(auto break_exp = dynamic_cast<ast::BreakExp*>(exp)){
        // TODO: ...
    }
        
    if(auto array_exp = dynamic_cast<ast::ArrayExp*>(exp)){
        // TODO: ...
    }
    
    // Error, it should have matched some clause
    assert(false);
};

void Translator::transDeclaration(ast::Declaration* dec){ // TODO: Modify and adapt to DeclarationList
    // TODO: Complete all the cases
    if(auto var_dec = dynamic_cast<ast::VarDec*>(dec)){
        // TODO: Check if correct
        auto result = transExpression(var_dec->exp.get());
        
        if(var_dec->type_id and getTypeEntry(*var_dec->type_id) != result.exp_type){
            // Error, type-id was explicitly specified but it doesn't match the expression type
            assert(false);
        }
        
        // TODO: Watch out for scope support
        insertValueEntry(*var_dec->id, new VarEntry(result.exp_type));
    }
    
    if(auto fun_dec = dynamic_cast<ast::FunDec*>(dec)){
        // TODO: ...
    }
    
    if(auto type_dec = dynamic_cast<ast::TypeDec*>(dec)){
        // TODO: ...
    }
    
    // Error, it should have matched some clause
    assert(false);
};

ExpType* Translator::transType(ast::Type* type){
    // TODO: Complete all the cases
    if(auto name_type = dynamic_cast<ast::NameType*>(type)){
        // TODO: ...
    }
        
    if(auto record_type = dynamic_cast<ast::RecordType*>(type)){
        // TODO: ...
    }
        
    if(auto array_type = dynamic_cast<ast::ArrayType*>(type)){
        // TODO: ...
    }
    
    // Error, it should have matched some clause
    assert(false);
};
