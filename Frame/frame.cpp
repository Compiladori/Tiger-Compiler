#include "frame.h"

/*
  Frame of x86_64 architecture with AMD64 ABI calling convention
  *      arg n       * <-%rbp + (n + 5) * 8
  *      arg 7       * <-%rbp + 16
  * return  adddress * <-%rbp + 8
  *   ------------   * <-%rbp
  *   static link    *<- %rbp -8
  *     local 1      *<- %rbp -16
  *     ----         *<- %rbp -24
  *     local n      *<- %rbp - 8 * n
*/

using namespace frame;
using namespace std;

int Frame::wordSize = 8;

Frame::Frame(temp::Label name, vector<bool> list) {
    _name = name;
    _offset = 0;
    for ( bool i : list ) {
        auto local = alloc_local(i);
        _formals.push_back(local);
    }
}

shared_ptr<Access> Frame::alloc_local(bool escape) {
    if ( escape ) {
        _offset -= Frame::wordSize;
        return make_shared<InFrame>(_offset);
    }
    return make_shared<InReg>();
}

temp::Temp Frame::ra_temp() {
    return Frame::get_reg_to_temp_map()["rax"];
}
temp::Temp Frame::rv_temp() {
    return Frame::get_reg_to_temp_map()["rax"];
}
temp::Temp Frame::fp_temp() {
    return Frame::get_reg_to_temp_map()["rbp"];
}
RegToTempMap Frame::register_temporaries;
RegList Frame::get_rets() { return RegList{"rax", "rdi", "rsi", "rdx", "rcx", "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15", "rbx", "rsp", "rbp"}; }
RegList Frame::get_arg_regs() { return RegList{"rdi", "rsi", "rdx", "rcx", "r8", "r9"}; }
RegList Frame::get_caller_saved_regs() { return RegList{"r10", "r11", "rcx", "rsi", "rip", "rdx", "rdi", "r8", "r9", "rax"}; }
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
    // static link at fp - 8
    return make_unique<irt::BinOp>(
        irt::Plus,
        move(framePtr),
        make_unique<irt::Const>(-Frame::wordSize));
}

unique_ptr<irt::Expression> frame::exp_with_static_link(shared_ptr<Access> acc, unique_ptr<irt::Expression> staticLink) {
    if ( auto in_reg = dynamic_cast<InReg *>(acc.get()) )
        return make_unique<irt::Temp>(in_reg->reg);

    if ( auto in_frame = dynamic_cast<InFrame *>(acc.get()) )
        return make_unique<irt::Mem>(
            make_unique<irt::BinOp>(
                irt::Plus,
                move(staticLink),
                make_unique<irt::Const>(in_frame->offset)));
}
unique_ptr<irt::Expression> frame::external_call(string s, unique_ptr<irt::ExpressionList> args) {
    return make_unique<irt::Call>(make_unique<irt::Name>(temp::Label(s)), move(args));
}

unique_ptr<irt::Statement> process_formal(std::shared_ptr<Access> formal, int arg_indx) {
    auto arg_temps = Frame::get_arg_regs();
    int arg_temps_length = arg_temps.size();
    auto temp_map = Frame::get_reg_to_temp_map();
    unique_ptr<irt::Expression> src, move_args;
    if ( arg_indx < arg_temps_length ) {
        src = make_unique<irt::Temp>(temp_map[arg_temps[arg_indx]]);
    } else {
        src = make_unique<irt::Mem>(
            make_unique<irt::BinOp>(irt::Plus,
                                    make_unique<irt::Const>((arg_indx - arg_temps_length + 2) * Frame::wordSize),
                                    make_unique<irt::Temp>(frame::Frame::fp_temp())));
    }

    return make_unique<irt::Move>(exp(formal, make_unique<irt::Temp>(frame::Frame::fp_temp())), move(src));
}
unique_ptr<irt::Statement> frame::proc_entry_exit1(shared_ptr<Frame> frame, unique_ptr<irt::Statement> stm) {
    auto formals = frame->formals();
    int arg_indx = 0;
    unique_ptr<irt::Seq> move_args;
    if ( !formals.size() ) {
        return move(stm);
    }
    for ( auto formal : formals ) {
        if ( move_args ) {
            move_args = make_unique<irt::Seq>(process_formal(formal, arg_indx), move(move_args));
        } else {
            move_args = make_unique<irt::Seq>(process_formal(formal, arg_indx), move(stm));
        }
        arg_indx++;
    }
    return move(move_args);
}

// This function appends a “sink” instruction to the function body to tell the
// register allocator that certain registers are live at procedure exit
assem::InstructionList frame::proc_entry_exit2(std::shared_ptr<Frame> frame, assem::InstructionList list) {
    RegList regs = frame->get_callee_saved_regs();
    auto reg_map = frame->get_reg_to_temp_map();
    temp::TempList return_sink;
    for ( auto &reg_name : regs ) {
        return_sink.push_back(Frame::register_temporaries[reg_name]);
    }
    return_sink.push_back(reg_map["rax"]);
    return_sink.push_back(reg_map["rsp"]);
    return_sink.push_back(reg_map["rsp"]);
    list.push_back(make_shared<assem::Oper>("", return_sink, temp::TempList{}, temp::LabelList{}));
    return list;
}

shared_ptr<assem::Procedure> frame::proc_entry_exit3(std::shared_ptr<Frame> frame, assem::InstructionList list) {
    RegList regs = frame->get_callee_saved_regs();
    auto reg_map = frame->get_reg_to_temp_map();
    string prolog = "# PROCEDURE " + (frame->_name).name + "\n";
    string epilog = "# END\n";
    int stack_pointer_offset = ((frame->_offset * -1 + 15) / 16) * 16;
    std::string offset_code = "subq $" + std::to_string(stack_pointer_offset) + ", %'s0";
    list.push_front(make_shared<assem::Oper>(offset_code, temp::TempList{reg_map["rsp"]}, temp::TempList{}, temp::LabelList{}));
    list.push_front(make_shared<assem::Oper>("movq %'s0, %'d0", temp::TempList{reg_map["rsp"]}, temp::TempList{reg_map["rbp"]}, temp::LabelList{}));
    list.push_front(make_shared<assem::Oper>("pushq  %'s0", temp::TempList{reg_map["rbp"]}, temp::TempList{reg_map["rsp"]}, temp::LabelList{}));
    list.push_front(make_shared<assem::Label>((frame->_name).name + ":", frame->_name));

    offset_code = "addq $" + std::to_string(stack_pointer_offset) + ", %'s0";
    list.push_back(make_shared<assem::Oper>(offset_code, temp::TempList{reg_map["rsp"]}, temp::TempList{reg_map["rsp"]}, temp::LabelList{}));
    list.push_back(make_shared<assem::Oper>("pop  %'s0", temp::TempList{reg_map["rbp"]}, temp::TempList{reg_map["rsp"]}, temp::LabelList{}));
    list.push_back(make_shared<assem::Oper>("ret", temp::TempList{}, temp::TempList{}, temp::LabelList{}));
    return make_shared<assem::Procedure>(prolog, list, epilog);
}
