#ifndef __MUNCH_H__
#define __MUNCH_H__

/**
 * Maximal munching
 * 
 * Described in Chapter 9 Appel C (2004)
 * **/

#include <memory>
#include "assem.h"
#include "../IRT/IRT.h"
#include "../Frame/temp.h"
#include "../Frame/frame.h"
#include "../Utility/utility.h"

namespace munch {



class Muncher {
    util::GenericList<assem::Instruction> instruction_list;
    frame::Frame munch_frame;
    
    void emit(std::unique_ptr<assem::Instruction> ins);

    void           munchStatement (irt::Statement*  stm);
    temp::TempList munchArgs(irt::ExpressionList* exp_list);
    temp::Temp     munchExpression(irt::Expression* exp);
public:
    Muncher(frame::Frame frame) : munch_frame(frame) {}
    
    util::GenericList<assem::Instruction> munchStatementList(irt::StatementList stm_list);
};



};

#endif
