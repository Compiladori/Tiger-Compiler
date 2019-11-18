#include "munch.h"
#include "../Utility/error.h"
#include "../Frame/temp.h"

using namespace munch;

/**
 * Muncher
 * **/

void emit(){
    // TODO: ...
    return;
}

void Muncher::munchStatement(irt::Statement* stm){
    // TODO: Complete all the cases
    if(auto seq_stm = dynamic_cast<irt::Seq*>(stm)){
        // ...
        return;
    }
    
    if(auto label_stm = dynamic_cast<irt::Label*>(stm)){
        // ...
        return;
    }
    
    if(auto jump_stm = dynamic_cast<irt::Jump*>(stm)){
        // ...
        return;
    }
    
    if(auto cjump_stm = dynamic_cast<irt::Cjump*>(stm)){
        // ...
        return;
    }
    
    if(auto move_stm = dynamic_cast<irt::Move*>(stm)){
        // ...
        return;
    }
    
    if(auto exp_stm = dynamic_cast<irt::Exp*>(stm)){
        // ...
        return;
    }
    
    // Internal error, it should have matched some clause of the above
    throw error::internal_error("Didn't match any clause while munching a statement", __FILE__);
}

temp::Temp Muncher::munchExpression(irt::Expression* exp){
    // TODO: Complete all the cases
    if(auto binop_exp = dynamic_cast<irt::BinOp*>(exp)){
        // ...
        return temp::Temp();
    }
    
    if(auto mem_exp = dynamic_cast<irt::Mem*>(exp)){
        // ...
        return temp::Temp();
    }
    
    if(auto temp_exp = dynamic_cast<irt::Temp*>(exp)){
        // ...
        return temp::Temp();
    }
    
    if(auto eseq_exp = dynamic_cast<irt::Eseq*>(exp)){
        // ...
        return temp::Temp();
    }
    
    if(auto name_exp = dynamic_cast<irt::Name*>(exp)){
        // ...
        return temp::Temp();
    }
    
    if(auto const_exp = dynamic_cast<irt::Const*>(exp)){
        // ...
        return temp::Temp();
    }
    
    if(auto call_exp = dynamic_cast<irt::Call*>(exp)){
        // ...
        return temp::Temp();
    }
    
    // Internal error, it should have matched some clause of the above
    throw error::internal_error("Didn't match any clause while munching a expression", __FILE__);
}


