#ifndef __FRAME_H__
#define __FRAME_H__

#include "temp.h"
#include <iostream>
#include <memory>
#include <vector>

/**
 * Frames for activaction records
 *
 * Described in Chapter 6 Appel C (2004)
 * **/

namespace frame {

/**
 * Forward declarations
 * **/
class Frame;
class Access;

using AccessList = util::GenericList<Access>;


class Frame {
    static const int wordSize;
    temp::Label _name;
    AccessList _formals;
    AccessList _locals;
    int _offset;

public:
    Frame(temp::Label f, std::vector<bool> list);
    temp::Label name(){ return _name;}
    AccessList& formals(){ return _formals;}
    std::unique_ptr<Access> alloc_local(bool escape);
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


};

#endif
