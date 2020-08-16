#ifndef __ASSEM_H__
#define __ASSEM_H__

/**
 * Abstract assembly-language instructions
 * 
 * Described in Chapter 9 Appel C (2004)
 * **/

#include <iostream>
#include <string>
#include <vector>

#include "../Frame/temp.h"

namespace assem {

struct Instruction {
    virtual ~Instruction() = default;
    virtual temp::TempList get_src() const = 0;
    virtual temp::TempList get_dst() const = 0;
    virtual void print(std::ostream& os, temp::TempMap& temp_map) const = 0;    // Instruction output
};

struct Oper : public Instruction {
    std::string assm;
    temp::TempList src, dst;
    temp::LabelList jumps;

    Oper(std::string assm, temp::TempList dst, temp::TempList src, temp::LabelList jumps) : assm(assm), src(src), dst(dst), jumps(jumps) {}
    virtual void print(std::ostream& os, temp::TempMap& temp_map) const override;
    virtual temp::TempList get_src() const { return src; };
    virtual temp::TempList get_dst() const { return dst; };
};

struct Label : public Instruction {
    std::string assm;
    temp::Label label;

    Label(std::string assm, temp::Label label) : assm(assm), label(label) {}
    virtual temp::TempList get_src() const { return temp::TempList(); };
    virtual temp::TempList get_dst() const { return temp::TempList(); };
    virtual void print(std::ostream& os, temp::TempMap& temp_map) const override;
};

struct Move : public Instruction {
    std::string assm;
    temp::TempList src, dst;

    Move(std::string assm, temp::TempList dst, temp::TempList src) : assm(assm), src(src), dst(dst) {}
    virtual temp::TempList get_src() const { return src; };
    virtual temp::TempList get_dst() const { return dst; };
    virtual void print(std::ostream& os, temp::TempMap& temp_map) const override;
};

};    // namespace assem

#endif
