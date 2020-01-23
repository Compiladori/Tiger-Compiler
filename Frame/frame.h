#ifndef __FRAME_H__
#define __FRAME_H__

#include "temp.h"
#include "../IRT/IRT.h"
#include <iostream>
#include <memory>
#include <vector>

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
std::unique_ptr<irt::Expression> static_link_jump(std::unique_ptr<irt::Expression> staticLink);
std::unique_ptr<irt::Expression> exp_with_static_link(std::shared_ptr<Access> acc, std::unique_ptr<irt::Expression> framePtr);
std::unique_ptr<irt::Expression> external_call(std::string s, std::unique_ptr<irt::ExpressionList> args);

using FragList = util::GenericList<Frag>;
using AccessList = std::vector<std::shared_ptr<Access>>;

class Frame {
    temp::Label _name;
    AccessList _formals;
    AccessList _locals;
    int _offset;
public:
    static int wordSize;
    static temp::Temp fp;
    Frame(temp::Label f, std::vector<bool> list);
    temp::Label name(){ return _name;}
    AccessList& formals(){ return _formals;}
    std::shared_ptr<Access> alloc_local(bool escape);
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
    StringFrag(temp::Label label,std::string str) : _label(label)  {}
    void print() const {}
};

// struct ProcFrag : public Frag {
//     Frame _frame;
//     std::unique_ptr<irt::Statement> body;
//     ProcFrag(Frame frame,std::unique_ptr<irt::Statement> body) : _frame(frame)  {}
//     void print() const {}
// };


};

#endif
