#include <cassert>
#include "escapes.h"

using namespace esc;

/**
 * Escapator
 * **/
// TODO: Replace assert() with custom error reporting

using std::make_unique, std::unique_ptr;
using std::move;

void Escapator::setEscapes(ast::Expression* exp){
    clear();
    traverseExpression(exp);
}

void Escapator::beginScope(){
    // Create a new scope without any initial insertions
    escape_insertions.push(std::stack<ast::Symbol>());
}

void Escapator::endScope(){
    if(escape_insertions.empty()){
        // Internal error, there is no scope to end
        assert(false);
    }
    
    // Remove all registered scoped insertions
    for(auto& escape_scope = escape_insertions.top(); not escape_scope.empty(); escape_scope.pop()){
        auto& symbol = escape_scope.top();
        EscapeEnv[symbol].pop();
    }
    escape_insertions.pop();
}

void Escapator::insertEscapeEntry(ast::Symbol s, std::unique_ptr<EscapeEntry> escape_entry, bool ignore_scope){
    if((not ignore_scope) and escape_insertions.empty()){
        // Internal error, no scope was initialized
        assert(false);
    }
    EscapeEnv[s].push(std::move(escape_entry));
    if(not ignore_scope)
        escape_insertions.top().push(s);
}


void Escapator::traverseExpression(ast::Expression* exp){
    // TODO: Complete all the cases
    if(auto var_exp = dynamic_cast<ast::VarExp*>(exp)){
        traverseVariable(var_exp->var.get());
        return;
    }
    
    // Nothing to do on these cases
    if(auto unit_exp = dynamic_cast<ast::UnitExp*>(exp))    { return; }
    if(auto nil_exp = dynamic_cast<ast::NilExp*>(exp))      { return; }
    if(auto int_exp = dynamic_cast<ast::IntExp*>(exp))      { return; }
    if(auto string_exp = dynamic_cast<ast::StringExp*>(exp)){ return; }
    
    if(auto call_exp = dynamic_cast<ast::CallExp*>(exp)){
        // TODO: ...
        return;
    }
    
    if(auto op_exp = dynamic_cast<ast::OpExp*>(exp)){
        traverseExpression(op_exp->left.get());
        traverseExpression(op_exp->right.get());
        return;
    }
    
    if(auto record_exp = dynamic_cast<ast::RecordExp*>(exp)){
        for(const auto& record_field : *record_exp->fields){
            traverseExpression(record_field->exp.get());
        }
        return;
    }
    
    if(auto seq_exp = dynamic_cast<ast::SeqExp*>(exp)){
        for(const auto& exp : *seq_exp->exp_list){
            traverseExpression(exp.get());
        }
        return;
    }
    
    if(auto assign_exp = dynamic_cast<ast::AssignExp*>(exp)){
        traverseVariable(assign_exp->var.get());
        traverseExpression(assign_exp->exp.get());
        return;
    }
    
    if(auto if_exp = dynamic_cast<ast::IfExp*>(exp)){
        traverseExpression(if_exp->test.get());
        traverseExpression(if_exp->then.get());
        if(if_exp->otherwise){
            traverseExpression(if_exp->otherwise.get());
        }
        return;
    }
    
    if(auto while_exp = dynamic_cast<ast::WhileExp*>(exp)){
        traverseExpression(while_exp->test.get());
        traverseExpression(while_exp->body.get());
        return;
    }
    
    if(auto for_exp = dynamic_cast<ast::ForExp*>(exp)){
        if(auto for_var = dynamic_cast<ast::SimpleVar*>(exp)){
            traverseExpression(for_exp->lo.get());
            traverseExpression(for_exp->hi.get());
            
            insertEscapeEntry(*for_var->id, make_unique<EscapeEntry>(current_depth, &for_exp->escape));
            traverseExpression(for_exp->body.get());
            return;
        }
        
        // Internal error, the for variable should have been a SimpleVar
        assert(false);
    }
    
    if(auto let_exp = dynamic_cast<ast::LetExp*>(exp)){
        // TODO: ...
        return;
    }
    
    if(auto break_exp = dynamic_cast<ast::BreakExp*>(exp)){ return; }
    
    if(auto array_exp = dynamic_cast<ast::ArrayExp*>(exp)){
        // TODO: ...
        return;
    }
    
    // Internal error, it should have matched some clause
    assert(false);
}

void Escapator::traverseDeclarations(ast::DeclarationList* dec_list){
    auto first_dec = dec_list->begin()->get();
    
    // TODO: Complete all the cases
    if(auto var_dec = dynamic_cast<ast::VarDec*>(first_dec)){
        // TODO: ...
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

void Escapator::traverseVariable(ast::Variable* var){
    if(auto simple_var = dynamic_cast<ast::SimpleVar*>(var)){
        // TODO: Check if correct
        if(auto escape_entry = getEscapeEntry(*simple_var->id)){
            // simple_var escape found
            if(current_depth > escape_entry->depth){
                *escape_entry->escape = true;
            }
        }
        return;
    }
    
    if(auto field_var = dynamic_cast<ast::FieldVar*>(var)){
        traverseVariable(field_var->var.get());
        return;
    }
    
    if(auto subscript_var = dynamic_cast<ast::SubscriptVar*>(var)){
        traverseVariable(subscript_var->var.get());
        traverseExpression(subscript_var->exp.get());
        return;
    }
    
    // Internal error, it should have matched some clause
    assert(false);
}
