#ifndef __FRAME_H__
#define __FRAME_H__
#include "temp.h"
#include <iostream>
#include <memory>
#include <vector>
namespace frame {

  class Frame;
  class FrameAccess;
  using FrameAccessList = util::GenericList<FrameAccess>;

class Frame {
  const static int wordSize;
  temp::Label _name;
  FrameAccessList _formals;
  FrameAccessList _locals;
  int _offset;
public:
    Frame(temp::Label f, std::vector<bool> list);
    temp::Label name(){ return _name;}
    FrameAccessList& formals(){ return _formals;}
    std::unique_ptr<FrameAccess> alloc_local(bool escape);
};

struct FrameAccess {
  virtual void print() const = 0;
};


struct InFrame : public FrameAccess{
  int offset;
  InFrame(int offset) : offset(offset) {}
  void print() const {}
};
struct InReg : public FrameAccess{
  temp::Temp reg;
  InReg() : reg(temp::Temp()) {}
  void print() const {}
};

};

#endif
