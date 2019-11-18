#ifndef __MUNCH_H__
#define __MUNCH_H__

/**
 * Maximal munching
 * 
 * Described in Chapter 9 Appel C (2004)
 * **/

#include <memory>
#include "../IRT/IRT.h"
#include "../Frame/temp.h"

namespace munch {



class Muncher {
    void emit();
public:
    Muncher() = default;
    // Stuff ...
    
    void       munchStatement (irt::Statement*  stm);
    temp::Temp munchExpression(irt::Expression* exp);
};



};

#endif
