#include "canon.h"

using namespace canon;


bool isNop(irt::Statement* stm){
    if (auto stmExp_ = dynamic_cast<irt::Exp*>(stm))
        if (dynamic_cast<irt::Const*>(stmExp_->exp.get()))
            return true;
    return false;
}

bool commute(irt::Statement* stm, irt::Expression* exp){
    if (dynamic_cast<irt::Name*>(exp))
        return true;
    if (dynamic_cast<irt::Const*>(exp))
        return true;
    return isNop(stm);
}

std::pair<irt::Statement*, ExpressionList*> reorder(ExpressionList expList){}

std::pair<irt::Statement*, ExpressionList*> doExp(irt::Expression* exp){
    if (auto binop = dynamic_cast<irt::BinOp*>(exp)){
        
    }
    if (auto mem = dynamic_cast<irt::Mem*>(exp)){

    }
    if (auto eseq = dynamic_cast<irt::Eseq*>(exp)){

    }
    if (auto call = dynamic_cast<irt::Call*>(exp)){

    }

}

std::unique_ptr<StatementList> linearize(std::unique_ptr<irt::Statement> stm){}
struct Block* basicBlocks(StatementList* stmList){}
StatementList* traceSchedule(Block block){}
