#include <map>
#include <set>
#include <string>

#include "../Frame/frame.h"
#include "../Frame/temp.h"
#include "../Utility/error.h"
#include "assem.h"
#include "munch.h"

using namespace munch;
using std::make_unique;
using std::move;
using std::unique_ptr;

/**
 * Corresponding assembly names to IRT's operations
 * **/

std::map<irt::BinaryOperation, std::string> binary_operation_code = {
    {irt::Plus, "addq"},
    {irt::Minus, "subq"},
    {irt::Mul, "imulq"},
    {irt::Div, "idivq"}};

std::map<irt::RelationOperation, std::string> relation_operation_code = {
    {irt::Eq, "je"},
    {irt::Ne, "jne"},
    {irt::Lt, "jl"},
    {irt::Gt, "jg"},
    {irt::Le, "jle"},
    {irt::Ge, "jge"}};

/**
 * Muncher
 * 
 * Uses assembly AT&T Syntax (same as GCC):
 * mnemonic source, destination
 * 
 * **/

void Muncher::emit(unique_ptr<assem::Instruction> ins) {
    instruction_list.push_back(move(ins));
}

void Muncher::munchStatement(irt::Statement* stm) {
    if ( auto seq_stm = dynamic_cast<irt::Seq*>(stm) ) {
        /* SEQ(left, right) */
        munchStatement(seq_stm->left.get());
        munchStatement(seq_stm->right.get());
        return;
    }

    if ( auto label_stm = dynamic_cast<irt::Label*>(stm) ) {
        /* LABEL(name) */
        emit(make_unique<assem::Label>(label_stm->label.name + ":", label_stm->label));
        return;
    }

    if ( auto jump_stm = dynamic_cast<irt::Jump*>(stm) ) {
        /* JUMP(exp, label_list) */
        if ( auto name_exp = dynamic_cast<irt::Name*>(jump_stm->exp.get()) ) {
            /* JUMP(NAME(label), label_list) */
            if ( jump_stm->label_list.size() != 1 )
                throw error::internal_error("Unexpected number of jump target labels while munching a JUMP statement", __FILE__);

            if ( not(jump_stm->label_list.front() == name_exp->name) )
                throw error::internal_error("Labels missmatch: JUMP statement does not jump to its label", __FILE__);

            /* JUMP(NAME(label), [label]) */
            emit(make_unique<assem::Oper>("jmp 'j0", temp::TempList{}, temp::TempList{}, jump_stm->label_list));
            return;
        }

        throw error::internal_error("Didn't match any JUMP clause while munching a statement", __FILE__);
    }

    if ( auto cjump_stm = dynamic_cast<irt::Cjump*>(stm) ) {
        /* CJUMP(relation_op, left, right, true_label, false_label) */
        std::string comp_code = "cmpq %'s0, %'s1";
        emit(make_unique<assem::Oper>(comp_code, temp::TempList{munchExpression(cjump_stm->left.get()), munchExpression(cjump_stm->right.get())}, temp::TempList{}, temp::LabelList{}));

        std::string jump_code = relation_operation_code[cjump_stm->rel_op] + " 'j0";
        emit(make_unique<assem::Oper>(jump_code, temp::TempList{}, temp::TempList{}, temp::LabelList{cjump_stm->true_label, cjump_stm->false_label}));
        return;
    }

    if ( auto move_stm = dynamic_cast<irt::Move*>(stm) ) {
        /* MOVE(...) */
        // Careful, the IRT is built using Intel syntax (move dst, src)
        // Swap the order position of the two move_stm expressions to reflect the AT&T syntax (move src, dst)
        std::string mov_code = "movq %'s0, (%'s1)";
        auto mem_dst_stm = dynamic_cast<irt::Mem*>(move_stm->left.get());
        if ( mem_dst_stm ) {
            auto mem_src_stm = dynamic_cast<irt::Mem*>(move_stm->right.get());
            if ( mem_src_stm ) {
                /* MOVE((MEM e1), (MEM e2)) */
                emit(make_unique<assem::Oper>(mov_code, temp::TempList{munchExpression(mem_src_stm->exp.get())}, temp::TempList{munchExpression(mem_dst_stm->exp.get())}, temp::LabelList{}));
                return;
            }
            mov_code = "movq %'s0, (%'s1)";
            emit(make_unique<assem::Oper>(mov_code, temp::TempList{munchExpression(move_stm->right.get()), munchExpression(mem_dst_stm->exp.get())}, temp::TempList{}, temp::LabelList{}));
            return;
        }
        auto temp_dst_stm = dynamic_cast<irt::Temp*>(move_stm->left.get());
        mov_code = "movq %'s0, %'d0";
        if ( temp_dst_stm ) {
            /* MOVE(TEMP(temporary), EXPRESSION(exp)) */
            emit(make_unique<assem::Move>(mov_code, temp::TempList{munchExpression(move_stm->right.get())}, temp::TempList{temp_dst_stm->temporary}));
            return;
        }

        throw error::internal_error("Didn't match any MOVE clause while munching a statement", __FILE__);
    }

    if ( auto exp_stm = dynamic_cast<irt::Exp*>(stm) ) {
        /* EXP(exp) */
        munchExpression(exp_stm->exp.get());
        return;
    }

    // Internal error, it should have matched some clause of the above
    throw error::internal_error("Didn't match any clause while munching a statement", __FILE__);
}

temp::TempList Muncher::munchArgs(irt::ExpressionList* args) {
    frame::RegToTempMap& reg_to_temp = munch_frame.get_reg_to_temp_map();

    auto arg_iterator = args->begin();
    temp::TempList argregsTemps;

    for ( auto& reg : munch_frame.get_arg_regs() ) {
        if ( arg_iterator == args->end() ) {
            // Ran out of function arguments
            break;
        }

        if ( !reg_to_temp.count(reg) )
            throw error::internal_error("Couldn't find x86-64 frame-provided argregs registers' temporaries while munching arguments from an EXP(CALL(...)) statement", __FILE__);
        argregsTemps.push_back(reg_to_temp[reg]);

        emit(make_unique<assem::Move>("movq %'s0, %'d0", temp::TempList{munchExpression(arg_iterator->get())}, temp::TempList{reg_to_temp[reg]}));
        arg_iterator++;
    }

    while ( arg_iterator != args->end() ) {
        // Ran out of frame argregs, pushing to stack
        // TODO : Warning, maybe it should push arguments in reversed order
        emit(make_unique<assem::Oper>("pushq %'s0", temp::TempList{munchExpression(arg_iterator->get())}, temp::TempList{}, temp::LabelList{}));
        arg_iterator++;
    }

    return argregsTemps;
}

temp::Temp Muncher::munchExpression(irt::Expression* exp) {
    if ( auto binop_exp = dynamic_cast<irt::BinOp*>(exp) ) {
        /* BINOP(bin_op, left, right) */
        if ( binop_exp->bin_op == irt::Plus or binop_exp->bin_op == irt::Minus ) {
            /* BINOP(Plus|Minus, left, right) */
            temp::Temp new_temporary;

            std::string mov_code = "movq %'s0, %'d0";
            emit(make_unique<assem::Move>(mov_code, temp::TempList{munchExpression(binop_exp->left.get())}, temp::TempList{new_temporary}));

            std::string bin_op_code = binary_operation_code[binop_exp->bin_op] + " %'s1, %'d0";
            emit(make_unique<assem::Oper>(bin_op_code, temp::TempList{new_temporary, munchExpression(binop_exp->right.get())}, temp::TempList{new_temporary}, temp::LabelList{}));

            return new_temporary;
        }

        if ( binop_exp->bin_op == irt::Mul or binop_exp->bin_op == irt::Div ) {
            /* BINOP(Mul|Div, left, right) */
            frame::RegToTempMap& reg_to_temp = munch_frame.get_reg_to_temp_map();
            if ( !reg_to_temp.count("rax") or !reg_to_temp.count("rdx") ) {
                throw error::internal_error("Couldn't find x86-64 frame-provided rax and rdx registers' temporaries while munching a BINOP expression", __FILE__);
            }
            temp::Temp new_temporary, rax = reg_to_temp["rax"], rdx = reg_to_temp["rdx"];

            emit(make_unique<assem::Move>("movq %'s0, %'d0", temp::TempList{munchExpression(binop_exp->left.get())}, temp::TempList{rax}));
            emit(make_unique<assem::Oper>("cqto", temp::TempList{rax}, temp::TempList{rdx}, temp::LabelList{}));

            std::string bin_op_code = binary_operation_code[binop_exp->bin_op] + " %'s2";
            emit(make_unique<assem::Oper>(bin_op_code, temp::TempList{rax, rdx, munchExpression(binop_exp->right.get())}, temp::TempList{rax, rdx}, temp::LabelList{}));
            emit(make_unique<assem::Move>("movq %'s0, %'d0", temp::TempList{rax}, temp::TempList{new_temporary}));    // Warning: ignoring information (such as multiplication overflow or division remainder)

            return new_temporary;
        }

        throw error::internal_error("Didn't match any BINOP clause while munching an expression", __FILE__);
    }

    if ( auto mem_exp = dynamic_cast<irt::Mem*>(exp) ) {
        /* MEM(exp) */
        temp::Temp new_temporary;
        std::string code = "movq (%'s0), %'d0";
        emit(make_unique<assem::Oper>(code, temp::TempList{munchExpression(mem_exp->exp.get())}, temp::TempList{new_temporary}, temp::LabelList{}));
        return new_temporary;
    }

    if ( auto temp_exp = dynamic_cast<irt::Temp*>(exp) ) {
        /* TEMP(temporary) */
        return temp_exp->temporary;
    }

    if ( auto eseq_exp = dynamic_cast<irt::Eseq*>(exp) ) {
        /* ESEQ(stm, exp) */
        throw error::internal_error("Unexpected Eseq while munching an expression: shouldn't be present in a canonized IRT", __FILE__);
    }

    if ( auto name_exp = dynamic_cast<irt::Name*>(exp) ) {
        /* NAME(label) */
        temp::Temp new_temporary;
        std::string code = "leaq " + name_exp->name.name + "(%rip), %'d0";
        emit(make_unique<assem::Oper>(code, temp::TempList{}, temp::TempList{new_temporary}, temp::LabelList{}));
        return new_temporary;
    }

    if ( auto const_exp = dynamic_cast<irt::Const*>(exp) ) {
        /* CONST(i) */
        temp::Temp new_temporary;
        std::string code = "movq $" + std::to_string(const_exp->i) + ", %'d0";
        emit(make_unique<assem::Oper>(code, temp::TempList{}, temp::TempList{new_temporary}, temp::LabelList{}));
        return new_temporary;
    }

    if ( auto call_exp = dynamic_cast<irt::Call*>(exp) ) {
        if ( auto fun_name_exp = dynamic_cast<irt::Name*>(call_exp->fun.get()) ) {
            /* EXP(CALL(LABEL(name), args = exp_list)) */
            frame::RegToTempMap& reg_to_temp = munch_frame.get_reg_to_temp_map();
            if ( !reg_to_temp.count("rax") or !reg_to_temp.count("rsp") )
                throw error::internal_error("Couldn't find x86-64 frame-provided rax and rsp registers' temporaries while munching an EXP(CALL(...)) statement", __FILE__);
            temp::Temp rax = reg_to_temp["rax"], rsp = reg_to_temp["rsp"];

            temp::TempList calldefsTemps;
            for ( auto& reg : munch_frame.get_calldefs() ) {
                if ( !reg_to_temp.count(reg) )
                    throw error::internal_error("Couldn't find x86-64 frame-provided calldefs registers' temporaries while munching an EXP(CALL(...)) statement", __FILE__);
                calldefsTemps.push_back(reg_to_temp[reg]);
            }

            int stack_pointer_offset = (call_exp->args->size() - munch_frame.get_arg_regs().size()) * munch_frame.wordSize;
            int stack_pointer_aligned = ((stack_pointer_offset + 15) / 16) * 16;    // Must be alligned to 16 bytes

            if ( stack_pointer_offset != stack_pointer_aligned ) {
                // Not aligned, add an extra argument to fix it
                // TODO : maybe duplicate the last argument?
                // call_exp->args->push_back(make_unique<irt::Temp>(munchExpression(call_exp->args->back().get())));

                // Adding the stack pointer as an extra last argument
                call_exp->args->push_back(make_unique<irt::Temp>(rsp));
            }
            auto calldefs = munch_frame.get_calldefs();
            for ( auto& reg : munch_frame.get_calldefs() ) {
                emit(make_unique<assem::Oper>("pushq %'s0 ", temp::TempList{reg_to_temp[reg]}, temp::TempList{rsp}, temp::LabelList{}));
            }

            std::string call_code = "call " + fun_name_exp->name.name;
            temp::TempList srcArgsTemps = munchArgs(call_exp->args.get());
            calldefsTemps.push_back(rax);
            emit(make_unique<assem::Oper>(call_code, srcArgsTemps, calldefsTemps, temp::LabelList{}));

            if ( stack_pointer_offset > 0 ) {
                std::string offset_code = "addq $" + std::to_string(stack_pointer_offset) + ", %'d0";
                emit(make_unique<assem::Oper>(offset_code, temp::TempList{rsp}, temp::TempList{rsp}, temp::LabelList{}));
            }
            for ( auto reg = calldefs.rbegin(); reg < calldefs.rend(); reg++ ) {
                emit(make_unique<assem::Oper>("popq %'d0 ", temp::TempList{rsp}, temp::TempList{reg_to_temp[*reg]}, temp::LabelList{}));
            }
            return rax;
        } else {
            throw error::internal_error("Unexpected non Name expression of FUN while munching a CALL statement", __FILE__);
        }
    }

    // Internal error, it should have matched some clause of the above
    throw error::internal_error("Didn't match any clause while munching an expression", __FILE__);
}

util::GenericList<assem::Instruction> Muncher::munchStatementList(irt::StatementList stm_list) {
    instruction_list.clear();

    for ( auto& stm : stm_list )
        munchStatement(stm.get());

    return move(instruction_list);
}