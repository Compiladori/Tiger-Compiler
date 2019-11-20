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
#include "../Utility/utility.h"

namespace munch {



class Muncher {
    util::GenericList<assem::Instruction> instruction_list;
    
    void emit(std::unique_ptr<assem::Instruction> ins);
public:
    Muncher() = default;
    // Stuff ...
    
    void       munchStatement (irt::Statement*  stm);
    temp::Temp munchExpression(irt::Expression* exp);
};



};

#endif
