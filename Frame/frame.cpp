#include "frame.h"

/*
  Frame of x86_64 architecture with AMD64 ABI calling convention
  *------------* <-%rbp
  *  saved rbp *<- %rbp -8
  *            *<- %rbp -16
  *            *<- %rbp -24
  *            *<- %rbp -32
*/
using namespace frame;
using namespace std;

const static int wordSize = 8;

Frame::Frame(temp::Label name, vector<bool> list){
  name = name;
  _offset = -16;
  frame::FrameAccessList a_list;
  for (bool i : list) {
    if(i){
      unique_ptr<InFrame> arg = make_unique<InFrame>(_offset);
      _offset -= 8;
      _formals.push_back(move(arg));
    }
    else{
      unique_ptr<InReg> arg = make_unique<InReg>();
      _formals.push_back(move(arg));
    }
  }
}

unique_ptr<FrameAccess> Frame::alloc_local(bool escape){
  if (escape) {
    unique_ptr<InFrame> l = make_unique<InFrame>(_offset);
    _offset -= 8;
    _locals.push_back(move(l));
    return l;
  }
  unique_ptr<InReg> l = make_unique<InReg>();
  _locals.push_back(move(l));
  return l;
}
