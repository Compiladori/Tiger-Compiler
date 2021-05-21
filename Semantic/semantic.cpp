#include "semantic.h"

#include <cassert>
#include <memory>
#include <stack>
#include <unordered_map>
#include <unordered_set>

#include "../AST/AST.h"
#include "../Translation/translatedExpression.h"
#include "../Utility/error.h"
#include "../Utility/toposort.h"

using namespace seman;
using namespace trans;
using namespace std;

/**
 * SemanticChecker
 * **/

using std::make_shared;
using std::make_unique;
using std::move;
using std::shared_ptr;
using std::unique_ptr;

unique_ptr<frame::FragList> SemanticChecker::translate(ast::Expression* exp) {
    auto t = temp::Label("tigermain");
    shared_ptr<trans::Level> outermost = make_shared<trans::Level>(nullptr, temp::Label("tigermain"), vector<bool>());
    clear(outermost);
    AssociatedExpType result = transExpression(outermost, exp);
    translator->proc_entry_exit(outermost, move(result.tr_exp));
    return move(translator->_frag_list);
}

void SemanticChecker::load_initial_values(shared_ptr<trans::Level> outermost) {
    auto TInt = make_shared<IntExpType>();
    auto TString = make_shared<StringExpType>();
    auto TUnit = make_shared<UnitExpType>();

    // Basic types
    insertTypeEntry("int", make_unique<TypeEntry>(TInt), true);
    insertTypeEntry("string", make_unique<TypeEntry>(TString), true);

    // Runtime functions
    using type_vector = vector<shared_ptr<ExpType>>;

    insertValueEntry("printi", make_unique<FunEntry>(type_vector{TInt}, TUnit, outermost, make_shared<temp::Label>("printi")), true);
    insertValueEntry("print", make_unique<FunEntry>(type_vector{TString}, TUnit, outermost, make_shared<temp::Label>("print")), true);
    insertValueEntry("flush", make_unique<FunEntry>(type_vector{}, TUnit, outermost, make_shared<temp::Label>("flush")), true);
    insertValueEntry("getchar", make_unique<FunEntry>(type_vector{}, TString, outermost, make_shared<temp::Label>("getchar")), true);
    insertValueEntry("ord", make_unique<FunEntry>(type_vector{TString}, TInt, outermost, make_shared<temp::Label>("ord")), true);
    insertValueEntry("chr", make_unique<FunEntry>(type_vector{TInt}, TString, outermost, make_shared<temp::Label>("chr")), true);
    insertValueEntry("size", make_unique<FunEntry>(type_vector{TString}, TInt, outermost, make_shared<temp::Label>("size")), true);
    insertValueEntry("substring", make_unique<FunEntry>(type_vector{TString, TInt, TInt}, TString, outermost, make_shared<temp::Label>("substring")), true);
    insertValueEntry("concat", make_unique<FunEntry>(type_vector{TString, TString}, TString, outermost, make_shared<temp::Label>("concat")), true);
    insertValueEntry("not", make_unique<FunEntry>(type_vector{TInt}, TInt, outermost, make_shared<temp::Label>("not")), true);
    insertValueEntry("exit", make_unique<FunEntry>(type_vector{TInt}, TUnit, outermost, make_shared<temp::Label>("exit")), true);

    lib_fun = {"printi", "print", "flush", "getchar", "ord", "chr", "size", "substring", "concat", "not", "exit"};
}
void SemanticChecker::beginScope() {
    // Create a new scope without any initial insertions
    type_insertions.push(stack<ast::Symbol>());
    value_insertions.push(stack<ast::Symbol>());
}

void SemanticChecker::endScope() {
    if ( type_insertions.empty() or value_insertions.empty() ) {
        // Internal error, there is no scope to end
        throw error::internal_error("there is no scope to end", __FILE__);
    }

    // Remove all registered scoped insertions
    for ( auto& type_scope = type_insertions.top(); not type_scope.empty(); type_scope.pop() ) {
        auto& symbol = type_scope.top();
        TypeEnv[symbol].pop();
    }
    type_insertions.pop();

    for ( auto& value_scope = value_insertions.top(); not value_scope.empty(); value_scope.pop() ) {
        auto& symbol = value_scope.top();
        ValueEnv[symbol].pop();
    }
    value_insertions.pop();
}
void SemanticChecker::endBreakScope() {
    if ( break_insertions.empty() ) {
        throw error::internal_error("There is no scope to end", __FILE__);
    }
    break_insertions.pop();
}

void SemanticChecker::insertTypeEntry(ast::Symbol s, unique_ptr<TypeEntry> type_entry, bool ignore_scope) {
    if ( (not ignore_scope) and type_insertions.empty() ) {
        // Internal error, no scope was initialized
        throw error::internal_error("no scope was initialized", __FILE__);
    }
    TypeEnv[s].push(move(type_entry));
    if ( not ignore_scope ) {
        type_insertions.top().push(s);
    }
}

void SemanticChecker::insertValueEntry(ast::Symbol s, unique_ptr<ValueEntry> value_entry, bool ignore_scope) {
    if ( (not ignore_scope) and value_insertions.empty() ) {
        // Internal error, no scope was initialized
        throw error::internal_error("no scope was initialized", __FILE__);
    }
    ValueEnv[s].push(move(value_entry));
    if ( not ignore_scope ) {
        value_insertions.top().push(s);
    }
}

AssociatedExpType SemanticChecker::transVariable(shared_ptr<trans::Level> lvl, ast::Variable* var) {
    if ( auto simple_var = dynamic_cast<ast::SimpleVar*>(var) ) {
        auto env_entry = getValueEntry(*simple_var->id);
        if ( auto var_entry = dynamic_cast<VarEntry*>(env_entry) ) {
            return AssociatedExpType(translator->simpleVar((var_entry->access), lvl), var_entry->type);
        }
        // Error, undefined variable
        throw error::semantic_error("Undefined variable \"" + simple_var->id->name + "\"", var->pos);
    }

    if ( auto field_var = dynamic_cast<ast::FieldVar*>(var) ) {
        auto var_result = transVariable(lvl, field_var->var.get());

        if ( auto record_type = dynamic_cast<RecordExpType*>(var_result.exp_type.get()) ) {
            int fieldIndex = 0;
            for ( const auto& field : record_type->fields ) {
                if ( field.name == field_var->id->name ) {
                    return AssociatedExpType(translator->fieldVar(move(var_result.tr_exp), fieldIndex), field.getType());
                }
                fieldIndex++;
            }
            // Error, id field doesn't exist
            throw error::semantic_error("Field \"" + field_var->id->name + "\" doesn't exist", var->pos);
        }
        // Error, expected record type
        throw error::semantic_error("Expected record type", var->pos);
    }

    if ( auto subscript_var = dynamic_cast<ast::SubscriptVar*>(var) ) {
        auto var_result = transVariable(lvl, subscript_var->var.get());
        if ( var_result.exp_type->kind != ExpTypeKind::ArrayKind ) {
            // Error, expected array type
            throw error::semantic_error("Expected array type", var->pos);
        }

        auto exp_result = transExpression(lvl, subscript_var->exp.get());
        if ( exp_result.exp_type->kind != ExpTypeKind::IntKind ) {
            // Error, expected int type in array index
            throw error::semantic_error("Expected int type in array index", var->pos);
        }

        auto array_result = static_cast<ArrayExpType*>(var_result.exp_type.get());
        return AssociatedExpType(translator->subscriptVar(move(var_result.tr_exp), move(exp_result.tr_exp)), array_result->getType());
    }

    // Internal error, it should have matched some clause
    throw error::internal_error("didn't match any clause in translate variable function", __FILE__);
}

AssociatedExpType SemanticChecker::transExpression(shared_ptr<trans::Level> lvl, ast::Expression* exp) {
    if ( auto var_exp = dynamic_cast<ast::VarExp*>(exp) ) {
        return transVariable(lvl, var_exp->var.get());
    }

    if ( /*auto unit_exp = */ dynamic_cast<ast::UnitExp*>(exp) ) {
        return AssociatedExpType(translator->unitExp(), make_shared<UnitExpType>());
    }

    if ( /*auto nil_exp = */ dynamic_cast<ast::NilExp*>(exp) ) {
        return AssociatedExpType(translator->nilExp(), make_shared<NilExpType>());
    }

    if ( auto int_exp = dynamic_cast<ast::IntExp*>(exp) ) {
        return AssociatedExpType(translator->intExp(int_exp), make_shared<IntExpType>());
    }

    if ( auto string_exp = dynamic_cast<ast::StringExp*>(exp) ) {
        return AssociatedExpType(translator->stringExp(string_exp), make_shared<StringExpType>());
    }

    if ( auto call_exp = dynamic_cast<ast::CallExp*>(exp) ) {
        auto env_entry = getValueEntry(*call_exp->func);
        if ( auto fun_entry = dynamic_cast<FunEntry*>(env_entry) ) {
            if ( fun_entry->formals.size() != call_exp->exp_list->size() ) {
                // Error, function call with a different number of arguments than the required ones
                throw error::semantic_error("Function \"" + call_exp->func->name + "\" called with a different number of arguments than the required ones", exp->pos);
            }
            unique_ptr<trans::ExpressionList> explist = make_unique<trans::ExpressionList>();
            for ( size_t index = 0; index < fun_entry->formals.size(); index++ ) {
                auto& param_type = fun_entry->formals[index];
                auto& arg_exp = (*call_exp->exp_list)[index];

                auto param_result = transExpression(lvl, arg_exp.get());
                explist->push_front(move(param_result.tr_exp));
                if ( *param_type != *param_result.exp_type ) {
                    // Error, the argument type doesn't match its expression type
                    throw error::semantic_error("Argument number " + to_string(index) + " has an unexpected type", exp->pos);
                }
            }
            return AssociatedExpType(translator->callExp(isFromLib((*call_exp->func).name), fun_entry->funlvl, lvl, *fun_entry->label, move(explist)), fun_entry->result);
        }
        // Error, the function wasn't declared in this scope
        throw error::semantic_error("Function \"" + call_exp->func->name + "\" wasn't declared in this scope", exp->pos);
    }

    if ( auto op_exp = dynamic_cast<ast::OpExp*>(exp) ) {
        auto oper = op_exp->oper;
        auto result_left = transExpression(lvl, op_exp->left.get());
        auto result_right = transExpression(lvl, op_exp->right.get());

        switch ( oper ) {
            case ast::Plus:
            case ast::Minus:
            case ast::Times:
            case ast::Divide: {
                if ( result_left.exp_type->kind != ExpTypeKind::IntKind ) {
                    // Error, integer required on the left
                    throw error::semantic_error("Operand on the left isn't an integer", exp->pos);
                }
                if ( result_right.exp_type->kind != ExpTypeKind::IntKind ) {
                    // Error, integer required on the right
                    throw error::semantic_error("Operand on the right isn't an integer", exp->pos);
                }
                return AssociatedExpType(translator->arExp(oper, move(result_left.tr_exp), move(result_right.tr_exp)), make_shared<IntExpType>());
            }
            case ast::Lt:
            case ast::Le:
            case ast::Gt:
            case ast::Ge: {
                auto left_kind = result_left.exp_type->kind;
                auto right_kind = result_right.exp_type->kind;

                if ( left_kind != right_kind ) {
                    // Error, operands' type kinds must be the same
                    throw error::semantic_error("Operands have different type", exp->pos);
                }
                if ( left_kind != ExpTypeKind::IntKind and left_kind != ExpTypeKind::StringKind ) {
                    // Error, operands' types must be between Int or String
                    throw error::semantic_error("Operands must have type Int or String", exp->pos);
                }
                if ( left_kind == ExpTypeKind::StringKind ) {
                    return AssociatedExpType(translator->strExp(oper, move(result_left.tr_exp), move(result_right.tr_exp)), make_shared<IntExpType>());
                }
                return AssociatedExpType(translator->condExp(oper, move(result_left.tr_exp), move(result_right.tr_exp)), make_shared<IntExpType>());
            }
            case ast::Eq:
            case ast::Neq: {
                if ( *result_left.exp_type != *result_right.exp_type ) {
                    // Error, different types on equality testing
                    throw error::semantic_error("Operands must have the same type", exp->pos);
                }
                if ( result_left.exp_type->kind == ExpTypeKind::StringKind )
                    return AssociatedExpType(translator->strExp(oper, move(result_left.tr_exp), move(result_right.tr_exp)), make_shared<IntExpType>());
                else
                    return AssociatedExpType(translator->condExp(oper, move(result_left.tr_exp), move(result_right.tr_exp)), make_shared<IntExpType>());
            }
        }

        // Internal error, the operator should have matched some clause of the switch
        throw error::internal_error("the operator didn't match any of the switch in translate expression function", __FILE__);
    }

    if ( auto record_exp = dynamic_cast<ast::RecordExp*>(exp) ) {
        if ( auto type_entry = getTypeEntry(*record_exp->type_id) ) {
            auto record_exp_type = dynamic_cast<RecordExpType*>(type_entry->type.get());
            if ( not record_exp_type ) {
                // Error, record type is undefined
                throw error::semantic_error("Record type \"" + record_exp->type_id->name + "\" undefined", exp->pos);
            }

            if ( record_exp_type->fields.size() != record_exp->fields->size() ) {
                // Error, different number of fields
                throw error::semantic_error("Wrong number of fields for record \"" + record_exp->type_id->name + "\"", exp->pos);
            }

            unordered_set<ast::Symbol, ast::SymbolHasher> declared_fields;
            int fieldCount = 0;
            unique_ptr<ExpressionList> field_list = make_unique<ExpressionList>();
            for ( const auto& field : *record_exp->fields ) {
                if ( declared_fields.count(*field->id) ) {
                    // Error, duplicated field name
                    throw error::semantic_error("Field \"" + field->id->name + "\" is duplicated", exp->pos);
                }

                declared_fields.insert(*field->id);

                auto getIndex = [&](const ast::Symbol& symbol) {
                    int index = 0;
                    for ( const auto& field : record_exp_type->fields ) {
                        if ( symbol.name == field.name ) {
                            return index;
                        }
                        index++;
                    }
                    return -1;
                };

                auto index = getIndex(*field->id);
                if ( index < 0 ) {
                    // Error, non existent field
                    throw error::semantic_error("Field \"" + field->id->name + "\" doesn't exist", exp->pos);
                }

                auto field_type = transExpression(lvl, field->exp.get());
                field_list->push_front(move(field_type.tr_exp));

                if ( *field_type.exp_type != *record_exp_type->fields[index].getType() ) {
                    // Error, field type doesn't match
                    throw error::semantic_error("Type of \"" + field->id->name + "\" doesn't match its definition", exp->pos);
                }
                fieldCount++;
            }
            auto a = translator->recordExp(move(field_list), fieldCount);
            // a -> print();
            return AssociatedExpType(move(a), type_entry->type);
        }

        // Error, record type was not defined
        throw error::semantic_error("Record \"" + record_exp->type_id->name + "\" wasn't defined in this scope", exp->pos);
    }

    if ( auto seq_exp = dynamic_cast<ast::SeqExp*>(exp) ) {
        const auto& list_ptr = seq_exp->exp_list;

        if ( list_ptr->empty() ) {
            // Internal error, the expression list shouldn't be empty
            throw error::internal_error("expression list is empty", __FILE__);
        }

        auto last_result = transExpression(lvl, list_ptr->back().get());
        unique_ptr<ExpressionList> seqlist = make_unique<ExpressionList>();
        for ( auto it = list_ptr->begin(); it != list_ptr->end()--; it++ ) {
            auto exp = transExpression(lvl, it->get());
            seqlist->push_front(move(exp.tr_exp));
        }

        return AssociatedExpType(translator->seqExp(move(seqlist)), last_result.exp_type);
    }

    if ( auto assign_exp = dynamic_cast<ast::AssignExp*>(exp) ) {
        auto var_result = transVariable(lvl, assign_exp->var.get());
        auto exp_result = transExpression(lvl, assign_exp->exp.get());

        if ( *var_result.exp_type != *exp_result.exp_type ) {
            // Error, variable's type is different than the expression's type
            throw error::semantic_error("Variable's type is different than the expression's type", exp->pos);
        }

        return AssociatedExpType(translator->assignExp(move(var_result.tr_exp), move(exp_result.tr_exp)), make_shared<UnitExpType>());
    }

    if ( auto if_exp = dynamic_cast<ast::IfExp*>(exp) ) {
        auto test_result = transExpression(lvl, if_exp->test.get());
        auto otherwise_result = AssociatedExpType(Translator::nullNx(), make_shared<UnitExpType>());

        if ( test_result.exp_type->kind != ExpTypeKind::IntKind ) {
            // Error, the if-test should be int
            throw error::semantic_error("The expression evaluated in the if clause must be an int", exp->pos);
        }

        auto then_result = transExpression(lvl, if_exp->then.get());
        if ( if_exp->otherwise ) {
            otherwise_result = transExpression(lvl, if_exp->otherwise.get());

            if ( *then_result.exp_type != *otherwise_result.exp_type ) {
                // Error, then and else clauses must be of the same type
                throw error::semantic_error("Then and Else clauses must be of the same type", exp->pos);
            }
        } else {
            if ( then_result.exp_type->kind != ExpTypeKind::UnitKind )
                throw error::semantic_error("if-then exp body must produce no value", exp->pos);
        }

        return AssociatedExpType(translator->ifExp(move(test_result.tr_exp), move(then_result.tr_exp), move(otherwise_result.tr_exp), then_result.exp_type.get()), then_result.exp_type);
    }

    if ( auto while_exp = dynamic_cast<ast::WhileExp*>(exp) ) {
        auto test_result = transExpression(lvl, while_exp->test.get());

        if ( test_result.exp_type->kind != ExpTypeKind::IntKind ) {
            // Error, the while-test should be int
            throw error::semantic_error("The expression evaluated in the while condition must be an int", exp->pos);
        }
        temp::Label breaklbl = temp::Label();

        beginScope();
        beginBreakScope(breaklbl);
        auto body_result = transExpression(lvl, while_exp->body.get());
        endBreakScope();
        endScope();

        if ( body_result.exp_type->kind != ExpTypeKind::UnitKind ) {
            // Error, the while-test should be int
            throw error::semantic_error("While body must produce no value", exp->pos);
        }
        return AssociatedExpType(translator->whileExp(move(test_result.tr_exp), move(body_result.tr_exp), breaklbl), make_shared<UnitExpType>());
    }

    if ( auto for_exp = dynamic_cast<ast::ForExp*>(exp) ) {
        beginScope();
        auto lo_result = transExpression(lvl, for_exp->lo.get());
        if ( lo_result.exp_type->kind != ExpTypeKind::IntKind ) {
            // Error, the for-lo should be int
            throw error::semantic_error("Initialization value (low) in for loop must be an int", exp->pos);
        }

        auto hi_result = transExpression(lvl, for_exp->hi.get());
        if ( hi_result.exp_type->kind != ExpTypeKind::IntKind ) {
            // Error, the for-hi should be int
            throw error::semantic_error("Final value (high) in for loop must be an int", exp->pos);
        }
        auto varentry = make_unique<VarEntry>(lo_result.exp_type, Level::alloc_local(lvl, for_exp->escape));
        auto access = varentry->access;
        insertValueEntry(for_exp->var->name, move(varentry));

        temp::Label forbreak = temp::Label();
        beginBreakScope(forbreak);
        auto body_result = transExpression(lvl, for_exp->body.get());
        endBreakScope();
        if ( body_result.exp_type->kind != ExpTypeKind::UnitKind ) {
            // Error, the for-lo should be int
            throw error::semantic_error("For body must produce no value", exp->pos);
        }
        endScope();
        // lo_result.tr_exp -> print();
        // hi_result.tr_exp -> print();
        // body_result.tr_exp -> print();
        return AssociatedExpType(translator->forExp(access, lvl, move(lo_result.tr_exp), move(hi_result.tr_exp), move(body_result.tr_exp), forbreak), make_shared<UnitExpType>());
    }

    if ( auto let_exp = dynamic_cast<ast::LetExp*>(exp) ) {
        beginScope();
        auto letlist = make_unique<ExpressionList>();
        for ( const auto& dec_list : *let_exp->decs ) {
            // Augment current scope by processing let declarations
            auto e = transDeclarations(lvl, dec_list.get());
            letlist->push_front(move(e));
        }
        auto result = transExpression(lvl, let_exp->body.get());
        // result.tr_exp -> print();
        endScope();
        return AssociatedExpType(translator->letExp(move(letlist), move(result.tr_exp)), result.exp_type);
    }

    if ( /*auto break_exp = */ dynamic_cast<ast::BreakExp*>(exp) ) {
        if ( BreakScopeEmpty() ) {
            throw error::semantic_error("Break must be in a loop", exp->pos);
        }
        auto breaklbl = getBreakEntry();
        return AssociatedExpType(translator->breakExp(breaklbl), make_shared<UnitExpType>());
    }

    if ( auto array_exp = dynamic_cast<ast::ArrayExp*>(exp) ) {
        if ( auto type_entry = getTypeEntry(*array_exp->ty) ) {
            auto array_type = dynamic_cast<ArrayExpType*>(type_entry->type.get());
            if ( not array_type ) {
                // Error, array type was not declared in this scope
                throw error::semantic_error("Array type \"" + array_exp->ty->name + "\" wasn't declared in this scope", exp->pos);
            }

            auto size_result = transExpression(lvl, array_exp->size.get());
            if ( size_result.exp_type->kind != ExpTypeKind::IntKind ) {
                // Error, array's size MUST be an int
                throw error::semantic_error("Array's size must be an int", exp->pos);
            }
            // size_result.tr_exp -> print();
            auto init_result = transExpression(lvl, array_exp->init.get());
            if ( *init_result.exp_type != *array_type->getType() ) {
                // Error, array type MUST match with its initialization's type
                throw error::semantic_error("Array type must match with its initialization type", exp->pos);
            }
            // init_result.tr_exp -> print();
            return AssociatedExpType(translator->arrayExp(move(init_result.tr_exp), move(size_result.tr_exp)), type_entry->type);
        }

        // Error, array type was not declared in this scope
        throw error::semantic_error("Array type \"" + array_exp->ty->name + "\" wasn't declared in this scope", exp->pos);
    }

    // Internal error, it should have matched some clause
    throw error::internal_error("didn't match any clause in translate expression function", __FILE__);
}

unique_ptr<TranslatedExp> SemanticChecker::transDeclarations(shared_ptr<trans::Level> lvl, ast::DeclarationList* dec_list) {
    if ( dec_list->empty() ) {
        // Internal error, declaration lists shouldn't be empty
        throw error::internal_error("declaration list is empty", __FILE__);
    }

    auto first_dec = dec_list->begin()->get();

    if ( auto var_dec = dynamic_cast<ast::VarDec*>(first_dec) ) {
        // var_dec -> print();
        if ( dec_list->size() != 1 ) {
            // Internal error, a declaration list of variables should only have one single element in it
            throw error::internal_error("declaration list of variables should have only one element", __FILE__);
        }

        auto result = transExpression(lvl, var_dec->exp.get());
        if ( var_dec->type_id ) {
            // Check if the explicitly specified type_id matches the type of the expression
            auto type_entry = getTypeEntry(*var_dec->type_id);
            if ( not type_entry ) {
                // Error, type_id wasn't declared in this scope
                throw error::semantic_error("Type \"" + var_dec->type_id->name + "\" wasn't declared in this scope", var_dec->pos);
            }

            auto var_type = type_entry->type.get();
            if ( *var_type != *result.exp_type ) {
                // Error, type_id was explicitly specified but doesn't match the expression type
                throw error::semantic_error("Type \"" + var_dec->type_id->name + "\" was explicitly specified but doesn't match the expression type", var_dec->pos);
            }
        }
        auto var_entry = make_unique<VarEntry>(result.exp_type, trans::Level::alloc_local(lvl, var_dec->escape));
        auto access = var_entry->access;
        insertValueEntry(*var_dec->id, move(var_entry));
        auto a = translator->simpleVar(access, lvl);
        // a -> print();
        // result.tr_exp -> print();
        return translator->assignExp(move(a), move(result.tr_exp));
    }

    if ( dynamic_cast<ast::FunDec*>(first_dec) ) {
        unordered_set<ast::Symbol, ast::SymbolHasher> declared_functions;

        // Insert a FunEntry for each function header
        for ( const auto& dec : *dec_list ) {
            auto fun_dec = static_cast<ast::FunDec*>(dec.get());

            if ( declared_functions.count(*fun_dec->id) ) {
                // Error, this function was already defined in the same scope
                throw error::semantic_error("Function \"" + fun_dec->id->name + "\" is already defined in this scope", fun_dec->pos);
            }

            declared_functions.insert(*fun_dec->id);

            shared_ptr<ExpType> return_type;
            if ( not fun_dec->type_id ) {
                return_type = make_shared<UnitExpType>();
            } else {
                auto return_type_entry = getTypeEntry(*fun_dec->type_id);
                if ( not return_type_entry ) {
                    // Error, the function return type wasn't declared
                    throw error::semantic_error("Function return type \"" + fun_dec->type_id->name + "\" wasn't declared", fun_dec->pos);
                }

                return_type = return_type_entry->type;
            }
            shared_ptr<temp::Label> label = make_shared<temp::Label>();
            unique_ptr<FunEntry> fun_entry = make_unique<FunEntry>(return_type, label);
            vector<bool> formals = vector<bool>();
            unordered_set<ast::Symbol, ast::SymbolHasher> declared_arguments;
            for ( const auto& type_field : *fun_dec->tyfields ) {
                if ( declared_arguments.count(*type_field->id) ) {
                    // Error, function arguments must have different names
                    throw error::semantic_error("Function arguments must have different names ", fun_dec->pos);
                }

                declared_arguments.insert(*type_field->id);
                formals.push_back(type_field->escape);
                if ( auto param_type_entry = getTypeEntry(*type_field->type_id) ) {
                    fun_entry->formals.push_back(param_type_entry->type);
                } else {
                    // Error, function parameter's type wasn't declared
                    throw error::semantic_error("Type of parameter \"" + type_field->id->name + "\" wasn't declared", fun_dec->pos);
                }
            }
            shared_ptr<trans::Level> funlvl = make_shared<trans::Level>(lvl, *label, formals);
            fun_entry->funlvl = funlvl;
            insertValueEntry(*fun_dec->id, move(fun_entry));
        }

        // Process each one of the function bodies, checking if the return type is actually correct
        for ( const auto& dec : *dec_list ) {
            auto fun_dec = static_cast<ast::FunDec*>(dec.get());

            if ( auto fun_entry = dynamic_cast<FunEntry*>(getValueEntry(*fun_dec->id)) ) {
                shared_ptr<Level> funlvl = fun_entry->funlvl;
                shared_ptr<AccessList> formals_list = trans::Level::formals(funlvl);
                beginScope();

                // Augment current scope with function arguments as new variables
                auto type_field = fun_dec->tyfields->begin();
                auto formal = formals_list->begin();
                while ( type_field != (*fun_dec->tyfields).end() && formal != formals_list->end() ) {
                    auto type_entry = getTypeEntry(*(*type_field)->type_id);
                    if ( not type_entry ) {
                        // Error, argument type not declared in this scope
                        throw error::semantic_error("Type of argument \"" + (*type_field)->type_id->name + "\" wasn't declared", fun_dec->pos);
                    }

                    insertValueEntry(*(*type_field)->id, make_unique<VarEntry>(type_entry->type, *formal));
                    formal++;
                    type_field++;
                }

                auto body_result = transExpression(funlvl, fun_dec->exp.get());
                if ( *fun_entry->result != *body_result.exp_type ) {
                    // Error, function return type doesn't match its body type
                    throw error::semantic_error("Function return type \"" + kind_name[fun_entry->result->kind] + "\" doesn't match its body type \"" + kind_name[body_result.exp_type->kind] + "\"", fun_dec->pos);
                }
                translator->proc_entry_exit(funlvl, move(body_result.tr_exp));
                endScope();
            } else {
                // Internal error, function's entry got somehow overriden or deleted
                throw error::internal_error("function's entry got somehow overriden or deleted", __FILE__);
            }
        }

        return Translator::NoExp();
    }

    if ( dynamic_cast<ast::TypeDec*>(first_dec) ) {
        tpsrt::Toposorter<ast::Symbol, ast::SymbolHasher> toposorter;
        unordered_map<ast::Symbol, ast::Type*, ast::SymbolHasher> symbol_to_ast_type;

        // First pass: check for duplicates and insert incomplete record / array types
        for ( const auto& dec : *dec_list ) {
            auto type_dec = static_cast<ast::TypeDec*>(dec.get());

            if ( symbol_to_ast_type.count(*type_dec->type_id) ) {
                // Error, this type was already defined in the same scope
                throw error::semantic_error("Type \"" + type_dec->type_id->name + "\" is already defined in this scope", type_dec->pos);
            }

            symbol_to_ast_type[*type_dec->type_id] = type_dec->ty.get();

            if ( auto record_type = dynamic_cast<ast::RecordType*>(type_dec->ty.get()) ) {
                shared_ptr<RecordExpType> record_exp_type = make_shared<RecordExpType>();

                for ( const auto& type_field : *record_type->tyfields ) {
                    record_exp_type->pushField(type_field->id->name, nullptr);
                }

                insertTypeEntry(*type_dec->type_id, make_unique<TypeEntry>(record_exp_type));
            }

            if ( /*auto array_type = */ dynamic_cast<ast::ArrayType*>(type_dec->ty.get()) ) {
                insertTypeEntry(*type_dec->type_id, make_unique<TypeEntry>(make_shared<ArrayExpType>()));
            }
        }

        // Second pass: toposort the simple NameType types
        for ( const auto& dec : *dec_list ) {
            auto type_dec = static_cast<ast::TypeDec*>(dec.get());

            if ( auto name_type = dynamic_cast<ast::NameType*>(type_dec->ty.get()) ) {
                // type a := name b  =>  b -> a
                toposorter.addDirectedEdge(*name_type->type_id, *type_dec->type_id);
            }
        }
        auto sorted_result = toposorter.sort();

        if ( not sorted_result.second.empty() ) {
            // Error, ilegal cycle detected in this scope
            throw error::semantic_error("Ilegal cycle detected in the scope starting", first_dec->pos);
        }

        for ( const auto& symbol : sorted_result.first ) {
            // Declare NameTypes in this scope
            if ( symbol_to_ast_type.count(symbol) ) {
                auto type_result = transType(lvl, symbol_to_ast_type[symbol]);
                insertTypeEntry(symbol, make_unique<TypeEntry>(type_result));
            }
        }

        // Third pass: fix record and array types
        for ( const auto& dec : *dec_list ) {
            auto type_dec = static_cast<ast::TypeDec*>(dec.get());

            if ( auto record_type = dynamic_cast<ast::RecordType*>(type_dec->ty.get()) ) {
                auto record_exptype = static_cast<RecordExpType*>(getTypeEntry(*type_dec->type_id)->type.get());

                for ( size_t i = 0; i < record_type->tyfields->size(); i++ ) {
                    auto field_type_entry = getTypeEntry(*(*record_type->tyfields)[i]->type_id);
                    if ( not field_type_entry ) {
                        // Error, record type not defined in this scope
                        throw error::semantic_error("Field record type \"" + kind_name[field_type_entry->type->kind] + "\" isn't defined in the scope", type_dec->pos);
                    }

                    record_exptype->updateField(i, field_type_entry->type);
                }
            }

            if ( auto array_type = dynamic_cast<ast::ArrayType*>(type_dec->ty.get()) ) {
                auto array_exptype = static_cast<ArrayExpType*>(getTypeEntry(*type_dec->type_id)->type.get());
                auto array_type_entry = getTypeEntry(*array_type->type_id);

                if ( not array_type_entry ) {
                    // Error, array type wasn't declared in this scope
                    throw error::semantic_error("Array type \"" + array_type->type_id->name + "\" wasn't declared in the scope", type_dec->pos);
                }

                array_exptype->updateType(array_type_entry->type);
            }
        }

        return Translator::NoExp();
    }

    // Internal error, it should have matched some clause
    throw error::internal_error("didn't match any clause in translate declarations function", __FILE__);
}

shared_ptr<ExpType> SemanticChecker::transType(shared_ptr<trans::Level> lvl, ast::Type* type) {
    if ( auto name_type = dynamic_cast<ast::NameType*>(type) ) {
        if ( auto type_entry = getTypeEntry(*name_type->type_id) ) {
            return type_entry->type;
        }

        // Error, type_id wasn't declared in this scope
        throw error::semantic_error("Type \"" + name_type->type_id->name + "\" wasn't declared in the scope", type->pos);
    }

    if ( auto record_type = dynamic_cast<ast::RecordType*>(type) ) {
        shared_ptr<RecordExpType> new_record_type = make_shared<RecordExpType>();
        unordered_set<ast::Symbol, ast::SymbolHasher> declared_fields;

        for ( const auto& type_field : *record_type->tyfields ) {
            if ( declared_fields.count(*type_field->id) ) {
                // Error, duplicated field declaration
                throw error::semantic_error("Duplicated field declaration for \"" + type_field->id->name + "\"", type->pos);
            }
            declared_fields.insert(*type_field->id);

            if ( auto type_entry = getTypeEntry(*type_field->type_id) ) {
                new_record_type->pushField(type_field->id->name, type_entry->type);
            } else {
                // Error, field type_id wasn't declared in this scope
                throw error::semantic_error("Field type \"" + type_field->type_id->name + "\" wasn't declared in the scope", type->pos);
            }
        }

        return new_record_type;
    }

    if ( auto array_type = dynamic_cast<ast::ArrayType*>(type) ) {
        if ( auto type_entry = getTypeEntry(*array_type->type_id) ) {
            return make_shared<ArrayExpType>(type_entry->type);
        }

        // Error, array's type_id wasn't declared in this scope
        throw error::semantic_error("Array's type \"" + array_type->type_id->name + "\" wasn't declared in the scope", type->pos);
    }

    // Internal error, it should have matched some clause
    throw error::internal_error("didn't match any clause in the translate type function", __FILE__);
}
