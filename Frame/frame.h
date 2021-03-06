#ifndef __FRAME_H__
#define __FRAME_H__

#include <iostream>
#include <map>
#include <memory>
#include <vector>

#include "../IRT/IRT.h"
#include "../Munch/assem.h"
#include "temp.h"

/**
 * Frames for activaction records
 *
 * Described in Chapter 6 Appel C (2004)
 * 
 * Frame class holds information about
 * formal parameters and local variables
 * 
 * Access represent formals and locals that
 * may be in the frame or in registers
 * **/

namespace frame {

/**
 * Forward declarations
 * **/
class Frame;
class Access;
class Frag;

std::unique_ptr<irt::Expression> exp(std::shared_ptr<Access> acc, std::unique_ptr<irt::Expression> framePtr);
std::unique_ptr<irt::Expression> static_link_exp_base(std::unique_ptr<irt::Expression> framePtr);
std::unique_ptr<irt::Expression> exp_with_static_link(std::shared_ptr<Access> acc, std::unique_ptr<irt::Expression> framePtr);
std::unique_ptr<irt::Expression> external_call(std::string s, std::unique_ptr<irt::ExpressionList> args);
std::unique_ptr<irt::Statement> proc_entry_exit1(std::shared_ptr<Frame> frame, std::unique_ptr<irt::Statement> stm);
assem::InstructionList proc_entry_exit2(std::shared_ptr<Frame> frame, assem::InstructionList list);
std::shared_ptr<assem::Procedure> proc_entry_exit3(std::shared_ptr<Frame> frame, assem::InstructionList list);

using FragList = util::GenericList<Frag>;
using AccessList = std::vector<std::shared_ptr<Access>>;
using Register = std::string;
using RegList = std::vector<Register>;
using RegToTempMap = std::map<Register, temp::Temp>;
using TempToRegMap = std::map<temp::Temp, Register>;

class Frame {
    AccessList _formals;

   public:
    int _offset;
    temp::Label _name;
    static int wordSize;
    Frame(temp::Label f, std::vector<bool> list);
    temp::Label name() { return _name; }
    AccessList& formals() { return _formals; }
    std::shared_ptr<Access> alloc_local(bool escape);
    std::shared_ptr<Access> alloc_helper(bool escape);
    RegList get_rets();
    static RegList get_arg_regs();
    static RegList get_caller_saved_regs();
    static RegList get_callee_saved_regs();
    static RegList get_calldefs();
    static RegToTempMap register_temporaries;
    static RegToTempMap& get_reg_to_temp_map();
    static temp::TempMap get_temp_to_reg_map();
    static temp::Temp ra_temp();
    static temp::Temp rv_temp();
    static temp::Temp fp_temp();
};

struct Access {
    virtual void print() const = 0;
};

struct InFrame : public Access {
    int offset;
    InFrame(int offset) : offset(offset) {}
    void print() const {}
};

struct InReg : public Access {
    temp::Temp reg;
    InReg() : reg(temp::Temp()) {}
    void print() const {}
};

struct Frag {
    virtual void print() const = 0;
};

struct StringFrag : public Frag {
    temp::Label _label;
    std::string str;
    StringFrag(temp::Label label, std::string str) : _label(label), str(str) {}
    void print() const {}
};

struct ProcFrag : public Frag {
    std::shared_ptr<Frame> _frame;
    std::unique_ptr<irt::Statement> body;
    ProcFrag(std::shared_ptr<Frame> frame, std::unique_ptr<irt::Statement> body) : _frame(frame), body(std::move(body)) {}
    void print() const {}
};

};    // namespace frame

#endif
