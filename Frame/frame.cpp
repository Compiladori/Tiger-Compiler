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
temp::Temp Frame::eax = temp::Temp();
temp::Temp Frame::ecx = temp::Temp();
temp::Temp Frame::edx = temp::Temp();
temp::Temp Frame::ebx = temp::Temp();
temp::Temp Frame::esi = temp::Temp();
temp::Temp Frame::edi = temp::Temp();
temp::Temp Frame::sp = temp::Temp();
temp::Temp Frame::zero = temp::Temp();
temp::Temp Frame::ra = temp::Temp();
temp::Temp Frame::rv = temp::Temp();

Frame::Frame(temp::Label name, vector<bool> list) {
    _name = name;
    _offset = -16;
    for ( bool i : list ) {
        if ( i ) {
            unique_ptr<InFrame> arg = make_unique<InFrame>(_offset);
            _offset -= 8;
            _formals.push_back(move(arg));
        } else {
            unique_ptr<InReg> arg = make_unique<InReg>();
            _formals.push_back(move(arg));
        }
    }
}

shared_ptr<Access> Frame::alloc_local(bool escape) {
    if ( escape ) {
        shared_ptr<InFrame> l = make_shared<InFrame>(_offset);
        _offset -= 8;
        _locals.push_back(l);
        return l;
    }
    shared_ptr<InReg> l = make_shared<InReg>();
    _locals.push_back(l);
    return l;
}

RegToTempMap Frame::register_temporaries;
RegList Frame::get_arg_regs() { return RegList{"rdi", "rsi", "rdx", "rcx", "r8", "r9"}; }
RegList Frame::get_caller_saved_regs() { return RegList{"rax", "r10", "r11"}; }
RegList Frame::get_callee_saved_regs() { return RegList{"r12", "r13", "r14", "r15", "rbx", "rsp", "rbp"}; }
RegList Frame::get_calldefs() { return Frame::get_caller_saved_regs(); }
RegToTempMap &Frame::get_reg_to_temp_map() {
    // Returns the static Frame::register_temporaries only if it's already filled up
    if ( Frame::register_temporaries.empty() ) {
        std::vector<std::string> register_names;

        RegList arg_regs = Frame::get_arg_regs();
        RegList caller_saved_regs = Frame::get_caller_saved_regs();
        RegList callee_saved_regs = Frame::get_callee_saved_regs();

        register_names.insert(register_names.end(), arg_regs.begin(), arg_regs.end());
        register_names.insert(register_names.end(), caller_saved_regs.begin(), caller_saved_regs.end());
        register_names.insert(register_names.end(), callee_saved_regs.begin(), callee_saved_regs.end());

        for ( auto &reg_name : register_names ) {
            // Assign a new temporary to each register
            Frame::register_temporaries[reg_name] = temp::Temp();
        }
    }
    return Frame::register_temporaries;
}

temp::TempMap Frame::get_temp_to_reg_map() {
    RegToTempMap reg_to_temp = get_reg_to_temp_map();
    temp::TempMap temp_to_reg;
    for ( auto const &pack : reg_to_temp )   
        temp_to_reg[pack.second] = temp::Label(pack.first);
    return temp_to_reg;
}

unique_ptr<irt::Expression> frame::exp(shared_ptr<Access> acc, unique_ptr<irt::Expression> framePtr) {
    if ( auto in_reg = dynamic_cast<InReg *>(acc.get()) )
        return make_unique<irt::Temp>(in_reg->reg);

    if ( auto in_frame = dynamic_cast<InFrame *>(acc.get()) )
        return make_unique<irt::Mem>(
            make_unique<irt::BinOp>(
                irt::Plus,
                move(framePtr),
                make_unique<irt::Const>(in_frame->offset)));
}

unique_ptr<irt::Expression> frame::static_link_exp_base(unique_ptr<irt::Expression> framePtr) {
    // static link at fp + 8
    return make_unique<irt::BinOp>(
        irt::Plus,
        move(framePtr),
        make_unique<irt::Const>(2 * Frame::wordSize));
}

unique_ptr<irt::Expression> frame::static_link_jump(unique_ptr<irt::Expression> staticLink) {
    return make_unique<irt::Mem>(move(staticLink));
}
unique_ptr<irt::Expression> frame::exp_with_static_link(shared_ptr<Access> acc, unique_ptr<irt::Expression> staticLink) {
    if ( auto in_reg = dynamic_cast<InReg *>(acc.get()) )
        return make_unique<irt::Temp>(in_reg->reg);

    if ( auto in_frame = dynamic_cast<InFrame *>(acc.get()) )
        return make_unique<irt::Mem>(
            make_unique<irt::BinOp>(
                irt::Plus,
                move(staticLink),
                make_unique<irt::Const>(in_frame->offset - 8)));
}
unique_ptr<irt::Expression> frame::external_call(string s, unique_ptr<irt::ExpressionList> args) {
    return make_unique<irt::Call>(make_unique<irt::Name>(temp::Label(s)), move(args));
}
unique_ptr<irt::Statement> frame::proc_entry_exit1(shared_ptr<Frame> frame, unique_ptr<irt::Statement> stm) {
    return move(stm);
}
unique_ptr<assem::Procedure> frame::proc_entry_exit3(assem::InstructionList list) {
    if ( auto head = dynamic_cast<assem::Label *>(list.front().get()) ) {
        string prolog = head->assm + "\npushl\t%ebp \nmovl\t%esp, %ebp \nsubl $64, %esp\n";
        list.pop_front();
        return make_unique<assem::Procedure>(prolog, move(list), "\tleave\n\tret\n");
    }
    exit(-1);    // every proc should always start with a label
}
