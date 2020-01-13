#include <string>
#include "munch.h"
#include "assem.h"
#include "../Utility/error.h"
#include "../Frame/temp.h"

using namespace munch;
using std::unique_ptr;
using std::make_unique;
using std::move;

/**
 * Corresponding assembly names to IRT's relation operators
 * **/
std::string relation_operation_name[] = { [irt::RelationOperation::Eq] = "je",
                                          [irt::RelationOperation::Ne] = "jne",
                                          [irt::RelationOperation::Lt] = "jl",
                                          [irt::RelationOperation::Gt] = "jg",
                                          [irt::RelationOperation::Le] = "jle",
                                          [irt::RelationOperation::Ge] = "jge",
                                          [irt::RelationOperation::Ult] = "jb",
                                          [irt::RelationOperation::Ule] = "jbe",
                                          [irt::RelationOperation::Ugt] = "ja",
                                          [irt::RelationOperation::Uge] = "jae" };

/**
 * Muncher
 * **/

void Muncher::emit(unique_ptr<assem::Instruction> ins){
    instruction_list.push_back(move(ins));
}

void Muncher::munchStatement(irt::Statement* stm){
    if(auto seq_stm = dynamic_cast<irt::Seq*>(stm)){
        /* SEQ(left, right) */
        munchStatement(seq_stm->left.get());
        munchStatement(seq_stm->right.get());
        return;
    }
    
    if(auto label_stm = dynamic_cast<irt::Label*>(stm)){
        /* LABEL(name) */
        std::string code = label_stm->label.name + ":";
        emit(make_unique<assem::Label>(code, label_stm->label));
        return;
    }
    
    if(auto jump_stm = dynamic_cast<irt::Jump*>(stm)){
        /* JUMP(exp) */
        temp::Temp munch_exp_result = munchExpression(jump_stm->exp.get());
        std::string code = "jmp `d0";
        emit(make_unique<assem::Oper>(code, temp::TempList {munch_exp_result}, temp::TempList {}, assem::Targets(jump_stm->label_list)));
        return;
    }
    
    if(auto cjump_stm = dynamic_cast<irt::Cjump*>(stm)){
        temp::Temp left = munchExpression(cjump_stm->left.get());
        temp::Temp right = munchExpression(cjump_stm->right.get());
        
        std::string comp_code = "cmp `s0, `s1";
        emit(make_unique<assem::Oper>(comp_code, temp::TempList {}, temp::TempList {left, right}, assem::Targets {}));
        
        std::string jump_code = relation_operation_name[cjump_stm->rel_op] + " `j0";
        emit(make_unique<assem::Oper>(jump_code, temp::TempList {}, temp::TempList {}, assem::Targets {*cjump_stm->true_label}));
        
        return;
    }
    
    if(auto move_stm = dynamic_cast<irt::Move*>(stm)){
        /* MOVE(...) */
        unique_ptr<irt::Expression>& move_dst = move_stm->left;
        
        if(auto mem_dst = dynamic_cast<irt::Mem*>(move_dst.get())){
            /* MOVE(MEM(...), e2) */
            
            if(auto binop_mem_dst = dynamic_cast<irt::BinOp*>(mem_dst->exp.get())){
                /* MOVE(MEM(BINOP(...)), e2) */
                
                if(auto binop_left_const = dynamic_cast<irt::Const*>(binop_mem_dst->left.get())){
                    /* MOVE(MEM(BINOP(OP, CONST(i), e1)), e2) */
                    
                    auto e1 = binop_mem_dst->right.get();
                    auto e2 = move_stm->right.get();
                    // TODO: set OP name accordingly and/or check what's actually allowed
                    std::string OP_name = "+";
                    std::string move_code = "mov [`s0 " + OP_name + " " + std::to_string(binop_left_const->i) + "], `s1";
                    emit(make_unique<assem::Move>(move_code, temp::TempList {}, 
                                                  temp::TempList {munchExpression(e1), munchExpression(e2)}));
                    return;
                }
                
                if(auto binop_right_const = dynamic_cast<irt::Const*>(binop_mem_dst->right.get())){
                    /* MOVE(MEM(BINOP(OP, e1, CONST(i))), e2) */
                    
                    auto e1 = binop_mem_dst->left.get();
                    auto e2 = move_stm->right.get();
                    // TODO: set OP name accordingly and/or check what's actually allowed
                    std::string OP_name = "+";
                    std::string move_code = "mov [`s0 " + OP_name + " " + std::to_string(binop_right_const->i) + "], `s1";
                    emit(make_unique<assem::Move>(move_code, temp::TempList {}, 
                                                  temp::TempList {munchExpression(e1), munchExpression(e2)}));
                    return;
                }
            }
            
            if(auto mem_const = dynamic_cast<irt::Const*>(mem_dst->exp.get())){
                /* MOVE(MEM(CONST(i)), e2) */
                
                auto e2 = move_stm->right.get();
                std::string move_code = "mov [`s0 + " + std::to_string(mem_const->i) +  "], `s1";
                
                emit(make_unique<assem::Move>(move_code, temp::TempList {}, temp::TempList {munchExpression(e2)}));
                return;
            }
            
            if(auto mem_src = dynamic_cast<irt::Mem*>(move_stm->right.get())){
                /* MOVE(MEM(e1), MEM(e2)) */
                
                auto e1 = mem_dst->exp.get();
                auto e2 = mem_src->exp.get();
                std::string move_code = "mov [`s0], `s1";
                
                emit(make_unique<assem::Move>(move_code, temp::TempList {}, temp::TempList {munchExpression(e1), munchExpression(e2)}));
                return;
            }
            
            /* MOVE(MEM(e1), e2) */
            auto e1 = mem_dst->exp.get();
            auto e2 = move_stm->right.get();
            std::string move_code = "mov [`s0], `s1";
            
            emit(make_unique<assem::Move>(move_code, temp::TempList {}, temp::TempList {munchExpression(e1), munchExpression(e2)}));
            return;
        }
        
        if(auto temp_dst = dynamic_cast<irt::Temp*>(move_dst.get())){
            /* MOVE(TEMP(t), e2) */
            
            auto e2 = move_stm->left.get();
            std::string move_code = "mov `d0, `s0";
            
            emit(make_unique<assem::Move>(move_code, 
                                          temp::TempList {munchExpression(temp_dst)},
                                          temp::TempList {munchExpression(e2)}));
            return;
        }
        
        throw error::internal_error("Didn't match any Move statement pattern", __FILE__);
    }
    
    if(auto exp_stm = dynamic_cast<irt::Exp*>(stm)){
        munchExpression(exp_stm->exp.get());
        return;
    }
    
    // Internal error, it should have matched some clause of the above
    throw error::internal_error("Didn't match any clause while munching a statement", __FILE__);
}

temp::Temp Muncher::munchExpression(irt::Expression* exp){
    // TODO: Complete all the cases
    if(auto binop_exp = dynamic_cast<irt::BinOp*>(exp)){
        // TODO: Complete
        return temp::Temp();
    }
    
    if(auto mem_exp = dynamic_cast<irt::Mem*>(exp)){
        // TODO: Complete
        return temp::Temp();
    }
    
    if(auto temp_exp = dynamic_cast<irt::Temp*>(exp)){
        /* TEMP(temporary) */
        return temp_exp->temporary;
    }
    
    if(auto eseq_exp = dynamic_cast<irt::Eseq*>(exp)){
        /* ESEQ(stm, exp) */
        munchStatement(eseq_exp->stm.get());
        return munchExpression(eseq_exp->exp.get());
    }
    
    if(auto name_exp = dynamic_cast<irt::Name*>(exp)){
        /* NAME(label) */
        temp::Temp new_temporary;
        temp_to_label[new_temporary] = name_exp->name;
        return new_temporary;
    }
    
    if(auto const_exp = dynamic_cast<irt::Const*>(exp)){
        /* CONST(i) */
        temp::Temp new_temporary;
        std::string code = "mov `d0, " + std::to_string(const_exp->i);
        emit(make_unique<assem::Move>(code, temp::TempList {new_temporary}, temp::TempList {}));
        return new_temporary;
    }
    
    if(auto call_exp = dynamic_cast<irt::Call*>(exp)){
        /* CALL(fun, args) */
        // TODO: Complete
        return temp::Temp();
    }
    
    // Internal error, it should have matched some clause of the above
    throw error::internal_error("Didn't match any clause while munching a expression", __FILE__);
}


