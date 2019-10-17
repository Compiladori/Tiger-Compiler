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
    clear();
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
    for(auto& type_scope = type_insertions.top(); not type_scope.empty(); type_scope.pop()){
        auto& symbol = type_scope.top();
        TypeEnv[symbol].pop();
    }
    type_insertions.pop();
    
    for(auto& value_scope = value_insertions.top(); not value_scope.empty(); value_scope.pop()){
        auto& symbol = value_scope.top();
        ValueEnv[symbol].pop();
    }
    value_insertions.pop();
}

void Translator::insertTypeEntry(ast::Symbol s, shared_ptr<trans::ExpType> type_entry, bool ignore_scope){
    if((not ignore_scope) and type_insertions.empty()){
        // Internal error, no scope was initialized
        assert(false);
    }
    TypeEnv[s].emplace(make_unique<TypeEntry>(type_entry));
    if(not ignore_scope){
        type_insertions.top().push(s);
    }
}

void Translator::insertValueEntry(ast::Symbol s, unique_ptr<ValueEntry> value_entry, bool ignore_scope){
    if((not ignore_scope) and value_insertions.empty()){
        // Internal error, no scope was initialized
        assert(false);
    }
    ValueEnv[s].push(move(value_entry));
    if(not ignore_scope){
        value_insertions.top().push(s);
    }
}


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
        auto result = transVariable(var_exp->var.get());
        return AssociatedExpType(make_shared<TranslatedExp>(), result.exp_type);
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
        auto env_entry = getValueEntry(*call_exp->func);
        if(auto fun_entry = dynamic_cast<trans::FunEntry*>(env_entry)){
            if(fun_entry->formals.size() != call_exp->exp_list->size()){
                // Error, function call with a different number of arguments than the required ones
                assert(false);
            }
            
            for(int index = 0; index < fun_entry->formals.size(); index++){
                auto& param_type = fun_entry->formals[index];
                auto& arg_exp = (*call_exp->exp_list)[index];
                
                auto param_result = transExpression(arg_exp.get());
                
                if(*param_type != *param_result.exp_type){
                    // Error, the argument type doesn't match its expression type
                    assert(false);
                }
            }
            
            return AssociatedExpType(make_shared<TranslatedExp>(), fun_entry->result);
        }
        // Error, the function wasn't declared in this scope
        assert(false);
    }
    
    if(auto op_exp = dynamic_cast<ast::OpExp*>(exp)){
        auto oper = op_exp->oper;
        auto result_left = transExpression(op_exp->left.get());
        auto result_right = transExpression(op_exp->right.get());
        
        switch(oper){
            case ast::Plus:
            case ast::Minus:
            case ast::Times: 
            case ast::Divide: {
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
            case ast::Lt:
            case ast::Le:
            case ast::Gt:
            case ast::Ge: {
                auto left_kind = result_left.exp_type->kind;
                auto right_kind = result_right.exp_type->kind;
                
                if(left_kind != right_kind){
                    // Error, type kinds must be the same
                    assert(false);
                }
                
                if(left_kind != ExpTypeKind::IntKind or left_kind != ExpTypeKind::StringKind){
                    // Error, operands must be between Int or String
                    assert(false);
                }
                
                return AssociatedExpType(make_shared<TranslatedExp>(), result_left.exp_type);
            }
            case ast::Eq:
            case ast::Neq: {
                if(*result_left.exp_type != *result_right.exp_type){
                    // Error, different types on equality testing
                    assert(false);
                }
                return AssociatedExpType(make_shared<TranslatedExp>(), result_left.exp_type);
            }
        }
        
        // Internal error, the operator should have matched some clause of the switch
        assert(false);
    }
    
    if(auto record_exp = dynamic_cast<ast::RecordExp*>(exp)){
        if(auto type_entry = getTypeEntry(*record_exp->type_id)){
            // TODO: Implement this, considering the order matters
        }
        // Error, record type not defined
        assert(false);
    }
    
    if(auto seq_exp = dynamic_cast<ast::SeqExp*>(exp)){
        auto& list_ptr = seq_exp->exp_list;
        
        if(list_ptr->size() == 0){
            // Internal error, the expression list shouldn't be empty
            assert(false);
        }
        
        auto last_result = transExpression((*list_ptr)[list_ptr->size()-1].get());
        return AssociatedExpType(make_shared<TranslatedExp>(), last_result.exp_type);
    }
    
    if(auto assign_exp = dynamic_cast<ast::AssignExp*>(exp)){
        auto var_result = transVariable(assign_exp->var.get());
        auto exp_result = transExpression(assign_exp->exp.get());
        
        if(*var_result.exp_type != *exp_result.exp_type){
            // Error, variable's type is different than the expression's type
            assert(false);
        }
        
        return AssociatedExpType(make_shared<TranslatedExp>(), make_shared<UnitExpType>());
    }
    
    if(auto if_exp = dynamic_cast<ast::IfExp*>(exp)){
        auto test_result = transExpression(if_exp->test.get());
        
        if(test_result.exp_type->kind != ExpTypeKind::IntKind){
            // Error, the if-test should be int
            assert(false);
        }        
        
        auto then_result = transExpression(if_exp->then.get());
        if(if_exp->otherwise){
            auto otherwise_result = transExpression(if_exp->otherwise.get());
            
            if(*then_result.exp_type != *otherwise_result.exp_type){
                // Error, then and else clauses must be of the same type
                assert(false);
            }
        }
        
        return AssociatedExpType(make_shared<TranslatedExp>(), then_result.exp_type);
    }
    
    if(auto while_exp = dynamic_cast<ast::WhileExp*>(exp)){
        auto test_result = transExpression(while_exp->test.get());
    
        if(test_result.exp_type->kind != ExpTypeKind::IntKind){
            // Error, the while-test should be int
            assert(false);
        }
        
        transExpression(while_exp->body.get());
        return AssociatedExpType(make_shared<TranslatedExp>(), make_shared<UnitExpType>());
    }
    
    if(auto for_exp = dynamic_cast<ast::ForExp*>(exp)){
        auto lo_result = transExpression(for_exp->lo.get());
        if(lo_result.exp_type->kind != ExpTypeKind::IntKind){
            // Error, the for-lo should be int
            assert(false);
        }
        
        auto hi_result = transExpression(for_exp->hi.get());
        if(hi_result.exp_type->kind != ExpTypeKind::IntKind){
            // Error, the for-hi should be int
            assert(false);
        }
        
        transExpression(for_exp->body.get());
        return AssociatedExpType(make_shared<TranslatedExp>(), make_shared<UnitExpType>());
    }
    
    if(auto let_exp = dynamic_cast<ast::LetExp*>(exp)){
        beginScope();
        for(const auto& dec_list : *let_exp->decs){
            // Augment current scope by processing let declarations
            transDeclarations(dec_list.get());
        }
        auto result = transExpression(let_exp->body.get());
        endScope();
        return result;
    }
    
    if(auto break_exp = dynamic_cast<ast::BreakExp*>(exp)){
        return AssociatedExpType(make_shared<TranslatedExp>(), make_shared<UnitExpType>());
    }
    
    if(auto array_exp = dynamic_cast<ast::ArrayExp*>(exp)){
        if(auto type_entry = getTypeEntry(*array_exp->ty)){
            auto size_result = transExpression(array_exp->size.get());
            if(size_result.exp_type->kind != ExpTypeKind::IntKind){
                // Error, array's size MUST be an int
                assert(false);
            }
            
            auto init_result = transExpression(array_exp->init.get());
            if(*init_result.exp_type != *type_entry->type){
                // Error, array type MUST match with its initialization's type
                assert(false);
            }
            
            return AssociatedExpType(make_shared<TranslatedExp>(), type_entry->type);
        }
        // Error, array's type was not declared
        assert(false);
    }
    
    // Internal error, it should have matched some clause
    assert(false);
}

void Translator::transDeclarations(ast::DeclarationList* dec_list){
    if(dec_list->empty()){
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
            if(*var_type != *result.exp_type) {
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
