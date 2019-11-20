#ifndef __ASSEM_H__
#define __ASSEM_H__

/**
 * Abstract assembly-language instructions
 * 
 * Described in Chapter 9 Appel C (2004)
 * **/

#include <iostream>
#include <vector>
#include <string>
#include "../Frame/temp.h"

namespace assem {

// TODO: fix type temp::Map
using Targets = temp::LabelList;

struct Instruction {
    virtual void print() const = 0;
    /*
    virtual void print(std::ostream& os, temp::Map temp_map) const = 0; // Instruction output
    * */
};

struct Oper : public Instruction {
    std::string assm;
    temp::TempList dst, src;
    Targets jumps;

    Oper(std::string assm, temp::TempList dst, temp::TempList src, Targets jumps) :
        assm(assm), dst(dst), src(src), jumps(jumps) {}
    
    /*
    virtual void print(std::ostream& os, temp::Map temp_map) const override;
    * */
    void print() const override;
};

struct Label : public Instruction {
    std::string assm;
    temp::Label label;

    Label(std::string assm, temp::Label label) : assm(assm), label(label) {}
    
    /*
    virtual void print(std::ostream& os, temp::Map temp_map) const override;
    * */
    void print() const override;
};

struct Move : public Instruction {
    std::string assm;
    temp::TempList dst, src;

    Move(std::string assm, temp::TempList dst, temp::TempList src) : 
        assm(assm), dst(dst), src(src) {}
    
    /*
    virtual void print(std::ostream& os, temp::Map temp_map) const override;
    * */
    void print() const override;
};


};

#endif