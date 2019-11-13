#include "canon.h"

using namespace canon;

auto linearize(irt::Statement* stm){}

auto basicBlocks(StatementList* stmList){}

StatementList* traceSchedule(Block block){}

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
