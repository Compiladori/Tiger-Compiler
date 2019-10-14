#include "translation.h"
#include "../AST/AST.h"

#include <cassert>
#include <stack>
#include <memory>

using namespace trans;

/**
 * Translator
 * **/
// TODO: Replace assert() with custom error reporting, including Position()

using std::unique_ptr, std::make_unique;
using std::shared_ptr, std::make_shared;
using std::move;

trans::AssociatedExpType Translator::translate(ast::Expression* exp){
    this->clear();
    return transExpression(exp);
}

void Translator::beginScope(){
    // Create a new scope without any initial insertions
    type_insertions.push(std::stack<ast::Symbol>());
    value_insertions.push(std::stack<ast::Symbol>());
}

void Translator::endScope(){
    if(type_insertions.empty() or value_insertions.empty()){
        // Internal error, there is no scope to end
        assert(false);
    }
    
    // Remove all registered scoped insertions
    while(not type_insertions.top().empty()){
        auto s = type_insertions.top().top();
        TypeEnv[s].pop();
        type_insertions.top().pop();
    }
    type_insertions.pop();
    
    while(not value_insertions.top().empty()){
        auto s = value_insertions.top().top();
        ValueEnv[s].pop();
        value_insertions.top().pop();
    }
    value_insertions.pop();
}

void Translator::insertTypeEntry(ast::Symbol s, shared_ptr<trans::ExpType> type_entry, bool ignore_scope){
    if((not ignore_scope) and type_insertions.empty()){
        // Error, no scope was initialized
        assert(false);
    }
    TypeEnv[s].emplace(make_unique<TypeEntry>(type_entry));
    if(not ignore_scope)
        type_insertions.top().push(s);
}

void Translator::insertValueEntry(ast::Symbol s, unique_ptr<ValueEntry> value_entry, bool ignore_scope){
    if((not ignore_scope) and value_insertions.empty()){
        // Error, no scope was initialized
        assert(false);
    }
    ValueEnv[s].push(move(value_entry));
    if(not ignore_scope)
        value_insertions.top().push(s);
}


// TODO: Check if the transSomething() return types are correct
AssociatedExpType Translator::transVariable(ast::Variable* var){
    // TODO: Complete all the cases
    if(auto simple_var = dynamic_cast<ast::SimpleVar*>(var)){
        auto env_entry = getValueEntry(*simple_var->id);
        if(auto var_entry = dynamic_cast<VarEntry*>(env_entry)){
            // TODO: Fix 'var_entry->type' to the actual type expected (Page 117-118 Appel C)
            return AssociatedExpType(make_shared<TranslatedExp>(), var_entry->type);
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
    
    // Internal error, it should have matched some clause
    assert(false);
}

AssociatedExpType Translator::transExpression(ast::Expression* exp){
    // TODO: Complete all the cases
    if(auto var_exp = dynamic_cast<ast::VarExp*>(exp)){
        // TODO: ...
    }
    
    if(auto unit_exp = dynamic_cast<ast::UnitExp*>(exp)){
        return AssociatedExpType(make_shared<TranslatedExp>(), make_shared<UnitExpType>());
    }
    
    if(auto nil_exp = dynamic_cast<ast::NilExp*>(exp)){
        return AssociatedExpType(make_shared<TranslatedExp>(), make_shared<NilExpType>());
    }
    
    if(auto int_exp = dynamic_cast<ast::IntExp*>(exp)){
        return AssociatedExpType(make_shared<TranslatedExp>(), make_shared<IntExpType>());
    }
    
    if(auto string_exp = dynamic_cast<ast::StringExp*>(exp)){
        return AssociatedExpType(make_shared<TranslatedExp>(), make_shared<StringExpType>());
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
                return AssociatedExpType(make_shared<TranslatedExp>(), make_shared<IntExpType>());
            }
            // TODO: Complete the rest of the operators
        }
        
        // Internal error, the operator should have matched some clause of the switch
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
    
    // Internal error, it should have matched some clause
    assert(false);
}

void Translator::transDeclarations(ast::DeclarationList* dec_list){
    if(dec_list->empty()){
        // TODO: Check if this is an actual error, or if we should simply do nothing instead
        // Internal error, declaration lists shouldn't be empty
        assert(false);
    }
    
    auto first_dec = dec_list->begin()->get();
    
    // TODO: Complete all the cases
    if(auto var_dec = dynamic_cast<ast::VarDec*>(first_dec)){
        auto result = transExpression(var_dec->exp.get());
        
        if(var_dec->type_id){
            // Check if the explicitly specified type_id matches the type of the expression
            auto var_type = getTypeEntry(*var_dec->type_id)->type.get();
            if(not var_type){
                // Error, type_id was not declared
                assert(false);
            }
            if(*var_type == *result.exp_type) {
                // Error, type-id was explicitly specified but doesn't match the expression type
                assert(false);
            }
        }
        
        insertValueEntry(*var_dec->id, make_unique<VarEntry>(result.exp_type));
        
        return;
    }
    
    if(auto fun_dec = dynamic_cast<ast::FunDec*>(first_dec)){
        // TODO: ...
        return;
    }
    
    if(auto type_dec = dynamic_cast<ast::TypeDec*>(first_dec)){
        // TODO: ...
        return;
    }
    
    // Internal error, it should have matched some clause
    assert(false);
}

std::shared_ptr<ExpType> Translator::transType(ast::Type* type){
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
    
    // Internal error, it should have matched some clause
    assert(false);
}
