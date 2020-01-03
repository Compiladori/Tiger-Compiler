#include "canon.h"

using namespace canon;


bool Canonizator::isNop(irt::Statement* stm){
    if (auto stmExp_ = dynamic_cast<irt::Exp*>(stm))
        if (dynamic_cast<irt::Const*>(stmExp_->exp.get()))
            return true;
    return false;
}

bool Canonizator::commute(irt::Statement* stm, irt::Expression* exp){
    if (dynamic_cast<irt::Name*>(exp))
        return true;
    if (dynamic_cast<irt::Const*>(exp))
        return true;
    return isNop(stm);
}

std::pair<irt::Statement*, ExpressionList*> Canonizator::reorder(ExpressionList* expList){
    for(auto e = expList->begin(); e != expList->end()--; e++){
        // llamo a doExp en cada expresion. Junto todos los stm que devuelven con SEQ(?
        // junto todas las expressions en una lista

    }

}

// Transforms a statement pointer into a unique pointer to that statement
std::unique_ptr<irt::Statement> Canonizator::makeUnique(irt::Statement* stm){
    std::unique_ptr<irt::Statement> stmU;
    if (auto seq = dynamic_cast<irt::Seq*>(stm)){
        stmU = std::make_unique<irt::Seq>(std::move(seq->left), std::move(seq->right));
        return stmU;
    }
    if (auto lab = dynamic_cast<irt::Label*>(stm)){
        stmU = std::make_unique<irt::Label>(lab->label);
        return stmU;
    }
    if (auto jmp = dynamic_cast<irt::Jump*>(stm)){
        stmU = std::make_unique<irt::Jump>(std::move(jmp->exp), jmp->label_list);
        return stmU;
    }
    if (auto cjmp = dynamic_cast<irt::Cjump*>(stm)){
        stmU = std::make_unique<irt::Cjump>(cjmp->rel_op, std::move(cjmp->left), std::move(cjmp->right),
                                            std::move(cjmp->true_label), std::move(cjmp->false_label));
        return stmU;
    }
    if (auto mov = dynamic_cast<irt::Move*>(stm)){
        stmU = std::make_unique<irt::Move>(std::move(mov->left), std::move(mov->right));
        return stmU;
    }
    if (auto expr = dynamic_cast<irt::Exp*>(stm)){
        stmU = std::make_unique<irt::Exp>(std::move(expr->exp));
        return stmU;
    }
}

irt::Statement* Canonizator::sequence(irt::Statement* stm1, irt::Statement* stm2){
    if (isNop(stm1))
        return stm2;
    if (isNop(stm2))
        return stm1;
    std::unique_ptr<irt::Statement> stm1U = makeUnique(stm1);
    std::unique_ptr<irt::Statement> stm2U = makeUnique(stm2);
    auto seqRes = new irt::Seq(move(stm1U), move(stm2U));
    return seqRes;
}

irt::Statement* Canonizator::doStm(irt::Statement* stm){
    if (auto seq = dynamic_cast<irt::Seq*>(stm)){
        irt::Statement* doLeft = doStm(seq->left.get());
        irt::Statement* doRight = doStm(seq->right.get());
        return sequence(doLeft, doRight);
    }
    if (auto jump = dynamic_cast<irt::Jump*>(stm)){
        ExpressionList* expList;
        expList->push_back(jump->exp.get());
        std::pair<irt::Statement*, ExpressionList*> p = reorder(expList);
        return sequence(p.first, jump);
    }
    if (auto cjump = dynamic_cast<irt::Cjump*>(stm)){
        ExpressionList* expList;
        expList->push_back(cjump->left.get());
        expList->push_back(cjump->right.get());
        std::pair<irt::Statement*, ExpressionList*> p = reorder(expList);
        return sequence(p.first, cjump);
    }
    if (auto move = dynamic_cast<irt::Move*>(stm)){
        // move->left == dst, move->right == src
        if (auto tmp = dynamic_cast<irt::Temp*>(move->left.get()))
            if (auto call = dynamic_cast<irt::Call*>(move->right.get())) {
            // get_call_rlist ?? :)))))))))))))))

        }
        else if (auto tmp = dynamic_cast<irt::Temp*>(move->left.get())){
            ExpressionList* expList;
            expList->push_back(move->right.get());
            std::pair<irt::Statement*, ExpressionList*> p = reorder(expList);
            return sequence(p.first, move);
        }
        else if (auto mem = dynamic_cast<irt::Mem*>(move->left.get())){
            ExpressionList* expList;
            expList->push_back(move->left.get());
            expList->push_back(move->right.get());
            std::pair<irt::Statement*, ExpressionList*> p = reorder(expList);
            return sequence(p.first, move);
        }
        else if (auto eseq = dynamic_cast<irt::Eseq*>(move->left.get())){
            irt::Statement* s = eseq->stm.get();
            irt::Statement* eseqNew = new irt::Move(std::move(eseq->exp), std::move(move->right));
            irt::Statement* seqNew = new irt::Seq(makeUnique(s), makeUnique(seqNew));
            return doStm(seqNew);
        }
    }
    if (auto expr = dynamic_cast<irt::Exp*>(stm)){
        // see what get_call_rlist means
    }
    return stm;
}

std::pair<irt::Statement*, irt::Expression*> Canonizator::doExp(irt::Expression* exp){
    if (auto binop = dynamic_cast<irt::BinOp*>(exp)){
        ExpressionList* expList;
        expList->push_back(binop->left.get());
        expList->push_back(binop->right.get());
        std::pair<irt::Statement*, ExpressionList*> p = reorder(expList);
        return std::make_pair(p.first, exp);
    }
    if (auto mem = dynamic_cast<irt::Mem*>(exp)){
        ExpressionList* expList;
        expList->push_back(mem->exp.get());
        std::pair<irt::Statement*, ExpressionList*> p = reorder(expList);
        return std::make_pair(p.first, exp);
    }
    if (auto eseq = dynamic_cast<irt::Eseq*>(exp)){
        std::pair<irt::Statement*, irt::Expression*> x = doExp(eseq->exp.get());
        irt::Statement* s = doStm(eseq->stm.get());
        if (isNop(s))
            return std::make_pair(x.first, x.second);
        if (isNop(x.first))
            return std::make_pair(s, x.second);
        return std::make_pair(sequence(s, x.first), x.second);
    }
    if (auto call = dynamic_cast<irt::Call*>(exp)){
        //return StmExp(reorder(get_call_rlist(exp)), exp);
        //qué mierda es get_call_rlist
    }
    std::pair<irt::Statement*, ExpressionList*> p = reorder(NULL);
    return std::make_pair(p.first, exp);
}

std::unique_ptr<StatementList> Canonizator::linearize(irt::Statement* stm){}
struct Block* Canonizator::basicBlocks(StatementList* stmList){}
StatementList* Canonizator::traceSchedule(Block block){}
