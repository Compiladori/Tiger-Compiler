#include "translation.h"
#include "../AST/AST.h"

#include <cassert>
#include <stack>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include "translation.h"
#include "../Utility/toposort.h"

using namespace trans;

/**
 * Translator
 * **/
// TODO: Replace assert() with custom error reporting, including Position()

using std::unique_ptr, std::make_unique;
using std::shared_ptr, std::make_shared;
using std::move;

AssociatedExpType Translator::translate(ast::Expression* exp){
    clear();
    return transExpression(exp);
}

void Translator::load_initial_values(){
    auto TInt    = make_shared<IntExpType>();
    auto TString = make_shared<StringExpType>();
    auto TUnit   = make_shared<UnitExpType>();

    // Basic types
    insertTypeEntry("int",    make_unique<TypeEntry>(TInt), true);
    insertTypeEntry("string", make_unique<TypeEntry>(TString), true);

    // Runtime functions
    using type_vector = std::vector<std::shared_ptr<ExpType>>;
    
    insertValueEntry("print",     make_unique<FunEntry>(type_vector{TString}, TUnit), true);
    insertValueEntry("flush",     make_unique<FunEntry>(type_vector{}, TUnit), true);
    insertValueEntry("getchar",   make_unique<FunEntry>(type_vector{}, TString), true);
    insertValueEntry("ord",       make_unique<FunEntry>(type_vector{TString}, TInt), true);
    insertValueEntry("chr",       make_unique<FunEntry>(type_vector{TInt}, TString), true);
    insertValueEntry("size",      make_unique<FunEntry>(type_vector{TString}, TInt), true);
    insertValueEntry("substring", make_unique<FunEntry>(type_vector{TString, TInt, TInt}, TString), true);
    insertValueEntry("concat",    make_unique<FunEntry>(type_vector{TString, TString}, TString), true);
    insertValueEntry("not",       make_unique<FunEntry>(type_vector{TInt}, TInt), true);
    insertValueEntry("exit",      make_unique<FunEntry>(type_vector{TInt}, TUnit), true);
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

void Translator::insertTypeEntry(ast::Symbol s, unique_ptr<TypeEntry> type_entry, bool ignore_scope){
    if((not ignore_scope) and type_insertions.empty()){
        // Internal error, no scope was initialized
        assert(false);
    }
    TypeEnv[s].push(move(type_entry));
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
    if(auto simple_var = dynamic_cast<ast::SimpleVar*>(var)){
        auto env_entry = getValueEntry(*simple_var->id);
        if(auto var_entry = dynamic_cast<VarEntry*>(env_entry)){
            return AssociatedExpType(make_shared<TranslatedExp>(), var_entry->type);
        }
        // Error, undefined variable
        assert(false);
    }

    if(auto field_var = dynamic_cast<ast::FieldVar*>(var)){
        auto var_result = transVariable(field_var->var.get());

        if(auto record_type = dynamic_cast<RecordExpType*>(var_result.exp_type.get())){
            for(const auto& field : record_type->fields){
                if(field.name == field_var->id->name){
                    return AssociatedExpType(make_shared<TranslatedExp>(), field.getType());
                }
            }
            // Error, id field doesn't exist
            assert(false);
        }
        // Error, expected record type
        assert(false);
    }

    if(auto subscript_var = dynamic_cast<ast::SubscriptVar*>(var)){
        auto var_result = transVariable(subscript_var->var.get());
        if(var_result.exp_type->kind != ExpTypeKind::ArrayKind){
            // Error, expected array type
            assert(false);
        }

        auto exp_result = transExpression(subscript_var->exp.get());
        if(exp_result.exp_type->kind != ExpTypeKind::IntKind){
            // Error, expected int type in array index
            assert(false);
        }

        return AssociatedExpType(make_shared<TranslatedExp>(), var_result.exp_type);
    }

    // Internal error, it should have matched some clause
    assert(false);
}

AssociatedExpType Translator::transExpression(ast::Expression* exp){
    if(auto var_exp = dynamic_cast<ast::VarExp*>(exp)){
        auto result = transVariable(var_exp->var.get());
        return AssociatedExpType(make_shared<TranslatedExp>(), result.exp_type);
    }

    if(/*auto unit_exp = */dynamic_cast<ast::UnitExp*>(exp)){
        return AssociatedExpType(make_shared<TranslatedExp>(), make_shared<UnitExpType>());
    }

    if(/*auto nil_exp = */dynamic_cast<ast::NilExp*>(exp)){
        return AssociatedExpType(make_shared<TranslatedExp>(), make_shared<NilExpType>());
    }

    if(/*auto int_exp = */dynamic_cast<ast::IntExp*>(exp)){
        return AssociatedExpType(make_shared<TranslatedExp>(), make_shared<IntExpType>());
    }

    if(/*auto string_exp = */dynamic_cast<ast::StringExp*>(exp)){
        return AssociatedExpType(make_shared<TranslatedExp>(), make_shared<StringExpType>());
    }

    if(auto call_exp = dynamic_cast<ast::CallExp*>(exp)){
        auto env_entry = getValueEntry(*call_exp->func);
        if(auto fun_entry = dynamic_cast<FunEntry*>(env_entry)){
            if(fun_entry->formals.size() != call_exp->exp_list->size()){
                // Error, function call with a different number of arguments than the required ones
                assert(false);
            }

            for(std::size_t index = 0; index < fun_entry->formals.size(); index++){
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
                    // Error, operands' type kinds must be the same
                    assert(false);
                }
                if(left_kind != ExpTypeKind::IntKind and left_kind != ExpTypeKind::StringKind){
                    // Error, operands' types must be between Int or String
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
            auto record_exp_type = dynamic_cast<RecordExpType*>(type_entry->type.get());
            if (not record_exp_type){
                // Error, record type is undefined
                assert(false);
            }

            if(record_exp_type->fields.size() != record_exp->fields->size()){
                // Error, different number of fields
                assert(false);
            }
            
            std::unordered_set<ast::Symbol, ast::SymbolHasher> declared_fields;
            for(const auto& field : *record_exp->fields){
                if(declared_fields.count(*field->id)){
                    // Error, duplicated field name
                    assert(false);
                }

                declared_fields.insert(*field->id);
                
                auto getIndex = [&](const ast::Symbol& symbol){
                    int index = 0;
                    for(const auto& field : record_exp_type->fields){
                        if(symbol.name == field.name){
                            return index;
                        }
                        index++;
                    }
                    return -1;
                };

                auto index = getIndex(*field->id);
                if(index < 0){
                    // Error, non existent field
                    assert(false);
                }

                auto field_type = transExpression(field->exp.get());
                
                if(*field_type.exp_type != *record_exp_type->fields[index].getType()){
                    // Error, field type doesn't match
                    assert(false);
                }
            }

            return AssociatedExpType(make_shared<TranslatedExp>(), type_entry->type);
        }

        // Error, record type was not defined
        assert(false);
    }

    if(auto seq_exp = dynamic_cast<ast::SeqExp*>(exp)){
        const auto& list_ptr = seq_exp->exp_list;

        if(list_ptr->empty()){
            // Internal error, the expression list shouldn't be empty
            assert(false);
        }

        auto last_result = transExpression(list_ptr->back().get());
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

    if(/*auto break_exp = */dynamic_cast<ast::BreakExp*>(exp)){
        return AssociatedExpType(make_shared<TranslatedExp>(), make_shared<UnitExpType>());
    }

    if(auto array_exp = dynamic_cast<ast::ArrayExp*>(exp)){
        if(auto type_entry = getTypeEntry(*array_exp->ty)){
            auto array_type = dynamic_cast<ArrayExpType*>(type_entry->type.get());
            if(not array_type){
                // Error, array type was not declared in this scope
                assert(false);
            }
            
            auto size_result = transExpression(array_exp->size.get());
            if(size_result.exp_type->kind != ExpTypeKind::IntKind){
                // Error, array's size MUST be an int
                assert(false);
            }

            auto init_result = transExpression(array_exp->init.get());
            if(*init_result.exp_type != *array_type->getType()){
                // Error, array type MUST match with its initialization's type
                assert(false);
            }

            return AssociatedExpType(make_shared<TranslatedExp>(), type_entry->type);
        }
        
        // Error, array type was not declared in this scope
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

    if(auto var_dec = dynamic_cast<ast::VarDec*>(first_dec)){
        if(dec_list->size() != 1){
            // Internal error, a declaration list of variables should only have one single element in it
            assert(false);
        }

        auto result = transExpression(var_dec->exp.get());

        if(var_dec->type_id){
            // Check if the explicitly specified type_id matches the type of the expression
            auto type_entry = getTypeEntry(*var_dec->type_id);
            if(not type_entry){
                // Error, type_id wasn't declared in this scope
                assert(false);
            }

            auto var_type = type_entry->type.get();
            if(*var_type != *result.exp_type) {
                // Error, type_id was explicitly specified but doesn't match the expression type
                assert(false);
            }
        }

        insertValueEntry(*var_dec->id, make_unique<VarEntry>(result.exp_type));

        return;
    }

    if(dynamic_cast<ast::FunDec*>(first_dec)){
        std::unordered_set<ast::Symbol, ast::SymbolHasher> declared_functions;

        // Insert a FunEntry for each function header
        for(const auto& dec : *dec_list){
            auto fun_dec = static_cast<ast::FunDec*>(dec.get());

            if(declared_functions.count(*fun_dec->id)){
                // Error, this function was already defined in the same scope
                assert(false);
            }

            declared_functions.insert(*fun_dec->id);

            shared_ptr<ExpType> return_type;
            if(not fun_dec->type_id){
                return_type = make_shared<UnitExpType>();
            } else {
                auto return_type_entry = getTypeEntry(*fun_dec->type_id);
                if(not return_type_entry){
                    // Error, the function return type wasn't declared
                    assert(false);
                }

                return_type = return_type_entry->type;
            }

            unique_ptr<FunEntry> fun_entry = make_unique<FunEntry>(return_type);
            
            std::unordered_set<ast::Symbol, ast::SymbolHasher> declared_arguments;
            for(const auto& type_field : *fun_dec->tyfields){
                if(declared_arguments.count(*type_field->id)){
                    // Error, function arguments must have different names
                    assert(false);
                }
                
                declared_arguments.insert(*type_field->id);
                
                if(auto param_type_entry = getTypeEntry(*type_field->type_id)){
                    fun_entry->formals.push_back(param_type_entry->type);
                } else {
                    // Error, function parameter's type wasn't declared
                    assert(false);
                }
            }

            insertValueEntry(*fun_dec->id, move(fun_entry));
        }

        // Process each one of the function bodies, checking if the return type is actually correct
        for(const auto& dec : *dec_list){
            auto fun_dec = static_cast<ast::FunDec*>(dec.get());
            
            if(auto fun_entry = dynamic_cast<FunEntry*>(getValueEntry(*fun_dec->id))){
                beginScope();
                
                // Augment current scope with function arguments as new variables
                for(const auto& type_field : *fun_dec->tyfields){
                    auto type_entry = getTypeEntry(*type_field->type_id);
                    if(not type_entry){
                        // Error, argument type not declared in this scope
                        assert(false);
                    }
                    
                    insertValueEntry(*type_field->id, make_unique<VarEntry>(type_entry->type));
                }
                
                auto body_result = transExpression(fun_dec->exp.get());
                if(*fun_entry->result != *body_result.exp_type){
                    // Error, function return type doesn't match its body type
                    assert(false);
                }
                
                endScope();
            } else {
                // Internal error, function's entry got somehow overriden or deleted
                assert(false);
            }
        }

        return;
    }

    if(dynamic_cast<ast::TypeDec*>(first_dec)){
        tpsrt::Toposorter<ast::Symbol, ast::SymbolHasher> toposorter;
        std::unordered_map<ast::Symbol, ast::Type*, ast::SymbolHasher> symbol_to_ast_type;

        // First pass: check for duplicates and insert incomplete record / array types
        for(const auto& dec : *dec_list){
            auto type_dec = static_cast<ast::TypeDec*>(dec.get());

            if(symbol_to_ast_type.count(*type_dec->type_id)){
                // Error, this type was already defined in the same scope
                assert(false);
            }

            symbol_to_ast_type[*type_dec->type_id] = type_dec->ty.get();

            if(auto record_type = dynamic_cast<ast::RecordType*>(type_dec->ty.get())){
                shared_ptr<RecordExpType> record_exp_type = make_shared<RecordExpType>();
                
                for(const auto& type_field : *record_type->tyfields){
                    record_exp_type->pushField(type_field->id->name, nullptr);
                } 
                
                insertTypeEntry(*type_dec->type_id, make_unique<TypeEntry>(record_exp_type));
            }

            if(/*auto array_type = */dynamic_cast<ast::ArrayType*>(type_dec->ty.get())){
                insertTypeEntry(*type_dec->type_id, make_unique<TypeEntry>(make_shared<ArrayExpType>()));
            }
        }

        // Second pass: toposort the simple NameType types
        for(const auto& dec : *dec_list){
            auto type_dec = static_cast<ast::TypeDec*>(dec.get());

            if(auto name_type = dynamic_cast<ast::NameType*>(type_dec->ty.get())){
                // type a := name b  =>  b -> a
                toposorter.addDirectedEdge(*name_type->type_id, *type_dec->type_id);
            }
        }
        auto sorted_result = toposorter.sort();
        
        if(not sorted_result.second.empty()){
            // Error, ilegal cycle detected in this scope
            assert(false);
        }

        for(const auto& symbol : sorted_result.first){
            // Declare NameTypes in this scope
            if(symbol_to_ast_type.count(symbol)){
                auto type_result = transType(symbol_to_ast_type[symbol]);
                insertTypeEntry(symbol, make_unique<TypeEntry>(type_result));
            }
        }

        // Third pass: fix record and array types
        for(const auto& dec : *dec_list){
            auto type_dec = static_cast<ast::TypeDec*>(dec.get());

            if(auto record_type = dynamic_cast<ast::RecordType*>(type_dec->ty.get())){
                auto record_exptype = static_cast<RecordExpType*>(getTypeEntry(*type_dec->type_id)->type.get());

                for(std::size_t i = 0; i < record_type->tyfields->size(); i++){
                    auto field_type_entry = getTypeEntry(*(*record_type->tyfields)[i]->type_id);
                    if(not field_type_entry){
                        // Error, record type not defined in this scope
                        assert(false);
                    }
                    
                    record_exptype->updateField(i, field_type_entry->type);
                }
            }

            if(auto array_type = dynamic_cast<ast::ArrayType*>(type_dec->ty.get())){
                auto array_exptype = static_cast<ArrayExpType*>(getTypeEntry(*type_dec->type_id)->type.get());
                auto array_type_entry = getTypeEntry(*array_type->type_id);
                
                if(not array_type_entry){
                    // Error, array type wasn't declared in this scope
                    assert(false);
                }

                array_exptype->updateType(array_type_entry->type);
            }
        }

        return;
    }

    // Internal error, it should have matched some clause
    assert(false);
}

shared_ptr<ExpType> Translator::transType(ast::Type* type){
    if(auto name_type = dynamic_cast<ast::NameType*>(type)){
        if(auto type_entry = getTypeEntry(*name_type->type_id)){
            return type_entry->type;
        }

        // Error, type_id wasn't declared in this scope
        assert(false);
    }

    if(auto record_type = dynamic_cast<ast::RecordType*>(type)){
        shared_ptr<RecordExpType> new_record_type = make_shared<RecordExpType>();
        std::unordered_set<ast::Symbol, ast::SymbolHasher> declared_fields;

        for(const auto& type_field : *record_type->tyfields){
            if(declared_fields.count(*type_field->id)){
                // Error, duplicated field declaration
                assert(false);
            }
            declared_fields.insert(*type_field->id);

            if(auto type_entry = getTypeEntry(*type_field->type_id)){
                new_record_type->pushField(type_field->id->name, type_entry->type);
            } else {
                // Error, field type_id wasn't declared in this scope
                assert(false);
            }
        }

        return new_record_type;
    }

    if(auto array_type = dynamic_cast<ast::ArrayType*>(type)){
        if(auto type_entry = getTypeEntry(*array_type->type_id)){
            return make_shared<ArrayExpType>(type_entry->type);
        }

        // Error, array's type_id wasn't declared in this scope
        assert(false);
    }

    // Internal error, it should have matched some clause
    assert(false);
}
