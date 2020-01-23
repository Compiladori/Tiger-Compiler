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


int Frame::wordSize = 8;
temp::Temp Frame::fp = temp::Temp();
static const temp::Temp eax = temp::Temp();
static const temp::Temp ecx = temp::Temp();
static const temp::Temp edx = temp::Temp();
static const temp::Temp ebx = temp::Temp();
static const temp::Temp esi = temp::Temp();
static const temp::Temp edi = temp::Temp();
static const temp::Temp sp = temp::Temp();
static const temp::Temp zero = temp::Temp();
static const temp::Temp ra = temp::Temp();
static const temp::Temp rv = temp::Temp();

Frame::Frame(temp::Label name, vector<bool> list){
    name = name;
    _offset = -16;
    AccessList a_list;
    for (bool i : list) {
        if(i){
            unique_ptr<InFrame> arg = make_unique<InFrame>(_offset);
            _offset -= 8;
            _formals.push_back(move(arg));
        } else {
            unique_ptr<InReg> arg = make_unique<InReg>();
            _formals.push_back(move(arg));
        }
    }
}

shared_ptr<Access> Frame::alloc_local(bool escape){
    if(escape) {
        shared_ptr<InFrame> l = make_shared<InFrame>(_offset);
        _offset -= 8;
        _locals.push_back(move(l));
        return l;
     }
    shared_ptr<InReg> l = make_unique<InReg>();
    _locals.push_back(move(l));
    return l;
}

unique_ptr<irt::Expression> frame::exp(shared_ptr<Access> acc, unique_ptr<irt::Expression> framePtr) {
    if(auto in_reg = dynamic_cast<InReg*>(acc.get()))
        return make_unique<irt::Temp>(in_reg->reg);
        
    if(auto in_frame = dynamic_cast<InFrame*>(acc.get()))
        return make_unique<irt::Mem>(
            make_unique<irt::BinOp>(
                irt::Plus, 
                move(framePtr),
                make_unique<irt::Const>(in_frame->offset)
            )
        );
}

unique_ptr<irt::Expression> frame::static_link_exp_base(unique_ptr<irt::Expression> framePtr) {
  // static link at fp + 8
  return make_unique<irt::BinOp>(
        irt::Plus,
        move(framePtr),
        make_unique<irt::Const>(2 * Frame::wordSize)
    );
}

unique_ptr<irt::Expression> frame::static_link_jump(unique_ptr<irt::Expression> staticLink) {
  return make_unique<irt::Mem>(move(staticLink));
}
unique_ptr<irt::Expression> frame::exp_with_static_link(shared_ptr<Access> acc, unique_ptr<irt::Expression> staticLink) {
    if(auto in_reg = dynamic_cast<InReg*>(acc.get()))
        return make_unique<irt::Temp>(in_reg->reg);

    if(auto in_frame = dynamic_cast<InFrame*>(acc.get()))
        return make_unique<irt::Mem>(
            make_unique<irt::BinOp>(
                irt::Plus, 
                move(staticLink),
                make_unique<irt::Const>(in_frame->offset - 8)
            )
        );
}
unique_ptr<irt::Expression> frame::external_call(string s, unique_ptr<irt::ExpressionList> args) {
  return make_unique<irt::Call>(make_unique<irt::Name>(temp::Label(s)), move(args));
}