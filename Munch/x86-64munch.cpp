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
        emit(make_unique<assem::Oper>(code, temp::TempList {munch_exp_result}, temp::TempList {}, jump_stm->label_list));
        return;
    }
    
    if(auto cjump_stm = dynamic_cast<irt::Cjump*>(stm)){
        // TODO: Complete
        return;
    }
    
    if(auto move_stm = dynamic_cast<irt::Move*>(stm)){
        // TODO: Complete
        return;
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
        // TODO: Complete
        return temp::Temp();
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


