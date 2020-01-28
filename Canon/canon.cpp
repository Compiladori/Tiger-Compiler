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

irt::Statement* Canonizator::reorder(ExpressionList* expList){
    if (!expList){
        std::unique_ptr<irt::Const> cte = std::make_unique<irt::Const>(0);
        irt::Exp* exp = new irt::Exp(std::move(cte));
        return exp;
    }
    else if (auto call = dynamic_cast<irt::Call*>(expList->front().get())){
        temp::Temp t = temp::Temp();
        std::unique_ptr<irt::Expression> expU = makeExpUnique(expList->front().get());
        std::unique_ptr<irt::Move> move = std::make_unique<irt::Move>(std::make_unique<irt::Temp>(t), std::move(expU));
        irt::Eseq* eseq = new irt::Eseq(std::move(move), std::make_unique<irt::Temp>(t));
        expList->pop_front();
        expList->push_front(eseq);
        return reorder(expList);
    }
    else {
        std::pair<irt::Statement*, irt::Expression*> head = doExp(expList->front().get());
        expList->pop_front();
        irt::Statement* tail = reorder(expList);
        if (commute(tail, head.second)){
            expList->push_front(head.second);
            return sequence(head.first, tail);
        }
        else {
            temp::Temp t = temp::Temp(); irt::Temp* tmp = new irt::Temp(t);
            expList->push_front(tmp);
            std::unique_ptr<irt::Expression> headExpU = makeExpUnique(head.second);
            irt::Move* move = new irt::Move(std::make_unique<irt::Temp>(t), std::move(headExpU));
            irt::Statement* s = sequence(move, tail);
            return sequence(head.first, s);
        }
    }
}

// Transforms an expression pointer into a unique pointer to that expression
std::unique_ptr<irt::Expression> Canonizator::makeExpUnique(irt::Expression* exp) {
    std::unique_ptr<irt::Expression> expU;
    if (auto binop = dynamic_cast<irt::BinOp*>(exp)){
        expU = std::make_unique<irt::BinOp>(binop->bin_op, std::move(binop->left), std::move(binop->right));
        return expU;
    }
    if (auto mem = dynamic_cast<irt::Mem*>(exp)){
        expU = std::make_unique<irt::Mem>(std::move(mem->exp));
        return expU;
    }
    if (auto temp = dynamic_cast<irt::Temp*>(exp)){
        expU = std::make_unique<irt::Temp>(temp->temporary);
        return expU;
    }
    if (auto eseq = dynamic_cast<irt::Eseq*>(exp)){
        expU = std::make_unique<irt::Eseq>(std::move(eseq->stm), std::move(eseq->exp));
        return expU;
    }
    if (auto name = dynamic_cast<irt::Name*>(exp)){
        expU = std::make_unique<irt::Name>(name->name);
        return expU;
    }
    if (auto cte = dynamic_cast<irt::Const*>(exp)){
        expU = std::make_unique<irt::Const>(cte->i);
        return expU;
    }
    if (auto call = dynamic_cast<irt::Call*>(exp)){
        expU = std::make_unique<irt::Call>(std::move(call->fun), std::move(call->args));
        return expU;
    }
}

// Transforms a statement pointer into a unique pointer to that statement
std::unique_ptr<irt::Statement> Canonizator::makeStmUnique(irt::Statement* stm) {
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
    std::unique_ptr<irt::Statement> stm1U = makeStmUnique(stm1);
    std::unique_ptr<irt::Statement> stm2U = makeStmUnique(stm2);
    auto seqRes = new irt::Seq(std::move(stm1U), std::move(stm2U));
    return seqRes;
}

// Argument exp will be a call expression
ExpressionList* Canonizator::getCallRList(irt::Expression* exp){
    if (auto call = dynamic_cast<irt::Call*>(exp)){
        ExpressionList* args = call->args.get();
        ExpressionList* expList;
        expList->push_back(call->fun.get());
        for(auto arg = args->begin(); arg != args->end()--; arg++)
            expList->push_back(arg->get());
        return expList;
    }
    return NULL; //  ???
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
        irt::Statement* p = reorder(expList);
        return sequence(p, jump);
    }
    if (auto cjump = dynamic_cast<irt::Cjump*>(stm)){
        ExpressionList* expList;
        expList->push_back(cjump->left.get());
        expList->push_back(cjump->right.get());
        irt::Statement* p = reorder(expList);
        return sequence(p, cjump);
    }
    if (auto move = dynamic_cast<irt::Move*>(stm)){
        // move->left == dst, move->right == src
        if (auto tmp = dynamic_cast<irt::Temp*>(move->left.get()))
            if (auto call = dynamic_cast<irt::Call*>(move->right.get())){
                ExpressionList* callList = getCallRList(move->right.get());
                irt::Statement* p = reorder(callList);
                return sequence(p, move);
            }
        else if (auto tmp = dynamic_cast<irt::Temp*>(move->left.get())){
            ExpressionList* expList;
            expList->push_back(move->right.get());
            irt::Statement* p = reorder(expList);
            return sequence(p, move);
        }
        else if (auto mem = dynamic_cast<irt::Mem*>(move->left.get())){
            ExpressionList* expList;
            expList->push_back(move->left.get());
            expList->push_back(move->right.get());
            irt::Statement* p = reorder(expList);
            return sequence(p, move);
        }
        else if (auto eseq = dynamic_cast<irt::Eseq*>(move->left.get())){
            irt::Statement* s = eseq->stm.get();
            irt::Statement* eseqNew = new irt::Move(std::move(eseq->exp), std::move(move->right));
            irt::Statement* seqNew = new irt::Seq(makeStmUnique(s), makeStmUnique(seqNew));
            return doStm(seqNew);
        }
    }
    if (auto expr = dynamic_cast<irt::Exp*>(stm)){
        ExpressionList* expList;
        if (auto call = dynamic_cast<irt::Call*>(expr->exp.get()))
            expList = getCallRList(call);
        else
            expList->push_back(expr->exp.get());
        irt::Statement* p = reorder(expList);
        return sequence(p, expr);
    }
    return stm;
}

// Given an expression e returns a statement s and an expression e1, where e1 contains
// no ESEQs, such that ESEQ(s, e1) would be equivalent to the original expression e
std::pair<irt::Statement*, irt::Expression*> Canonizator::doExp(irt::Expression* exp){
    if (auto binop = dynamic_cast<irt::BinOp*>(exp)){
        ExpressionList* expList;
        expList->push_back(binop->left.get());
        expList->push_back(binop->right.get());
        irt::Statement* p = reorder(expList);
        return std::make_pair(p, exp);
    }
    if (auto mem = dynamic_cast<irt::Mem*>(exp)){
        ExpressionList* expList;
        expList->push_back(mem->exp.get());
        irt::Statement* p = reorder(expList);
        return std::make_pair(p, exp);
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
        ExpressionList* expList = getCallRList(call);
        irt::Statement* p = reorder(expList);
        return std::make_pair(p, exp);
    }
    irt::Statement* p = reorder(NULL);
    return std::make_pair(p, exp);
}

StatementList* Canonizator::linear(irt::Statement* stm, StatementList* right){
    if (auto seq = dynamic_cast<irt::Seq*>(stm))
        return linear(seq->left.get(), linear(seq->right.get(), right));
    else {
        StatementList* stmList;
        stmList->push_back(stm);
        for(auto s = right->begin(); s != right->end()--; s++)
            stmList->push_back(s->get());
        return stmList;
    }
}

StatementList* Canonizator::linearize(irt::Statement* stm){
    return linear(doStm(stm), NULL);
}

StatementListList* Canonizator::createBlocks(StatementList* stmList, temp::Label done){
    if (!stmList)
        return NULL;
    if (auto lab = dynamic_cast<irt::Label*>(stmList->front().get())){
        StatementList* tail = stmList; tail->pop_front();
        StatementListList* res = next(stmList, tail, done);
        res->push_front(stmList);
        return res;
    }
    temp::Label l = temp::Label();
    std::unique_ptr<irt::Label> lab = std::make_unique<irt::Label>(l);
    stmList->push_front(std::move(lab));
    return createBlocks(stmList, done);
}

StatementListList* Canonizator::next(StatementList* prevStm, StatementList* stm, temp::Label done){
    if (!stm){ // “special” last block – put a JUMP(NAME done) at the end of the last block.
      StatementList* stmListJump = new StatementList();
      temp::LabelList label_list; label_list.push_back(done);
      irt::Jump* jump = new irt::Jump(std::make_unique<irt::Name>(done), label_list);
      stmListJump->push_back(jump);
      return next(prevStm, stmListJump, done);
    }
    if (dynamic_cast<irt::Jump*>(stm->front().get()) or dynamic_cast<irt::Cjump*>(stm->front().get())){
      StatementListList* stmLists;
      irt::Statement* tmp = prevStm->front().get(); prevStm->pop_front();
      prevStm = stm; prevStm->push_front(tmp);
      StatementList* tail = stm; tail->pop_front();
      stmLists = createBlocks(tail, done);
      tmp = stm->front().get(); stm->pop_front();
      stm = NULL; stm->push_front(tmp);
      return stmLists;
    }
    else if (auto label = dynamic_cast<irt::Label*>(stm->front().get())){
      temp::Label lab = label->label;
      StatementList* newStm = stm;
      temp::LabelList label_list; label_list.push_back(lab);
      irt::Jump* jump = new irt::Jump(std::make_unique<irt::Name>(lab), label_list);
      newStm->push_front(jump);
      return next(prevStm, newStm, done);
    }
    else {
      irt::Statement* tmp = prevStm->front().get(); prevStm->pop_front();
      prevStm = stm; prevStm->push_front(tmp);
      StatementList* tail = stm; tail->pop_front(); // check
      return next(stm, tail, done);
    }
}

struct Block* Canonizator::basicBlocks(StatementList* stmList){
    temp::Label label = temp::Label();
    StatementListList* stmLists = createBlocks(stmList, label);
    struct Block* block = new Block(stmLists, label);
    return block;
}

void Canonizator::trace(StatementList* stmList){
    StatementList* lastTwo = stmList, *tmp;
    for (; lastTwo->size() > 2; lastTwo->pop_front());
    tmp = lastTwo; tmp->pop_front();
    irt::Statement* last = tmp->front().get(); // s == last, last == lasttwo
    auto label = dynamic_cast<irt::Label*>(stmList->front().get());
    std::pair<StatementList*, temp::Label> pair = std::make_pair((StatementList*) NULL, label->label);
    q->push_back(&pair);
    if (auto jump = dynamic_cast<irt::Jump*>(last)){
        StatementList* lab = NULL;
        temp::LabelList tailLabList(jump->label_list.begin()+1, jump->label_list.end());
        for (auto stm = q->begin(); stm != q->end(); stm++){
            if (jump->label_list.front() == stm->get()->second and
                !tailLabList.size()){
                  irt::Statement* head = lastTwo->front().get(); lastTwo->pop_front();
                  lastTwo = stm->get()->first; lastTwo->push_front(head); //remove jump
                  trace(stm->get()->first);
                }
        }
        StatementList* tailtail = getNext();
        for (auto stm = tailtail->begin(); stm != tailtail->end(); stm++)
            lastTwo->push_back(stm->get());
    }
    else if (auto cjump = dynamic_cast<irt::Cjump*>(last)){
        // look for the true and false label
        StatementList* trueLabel = NULL, *falseLabel = NULL;
        for (auto stm = q -> begin(); stm != q -> end(); stm++){
            if (stm->get()->second == *(cjump->true_label))
                trueLabel = stm->get()->first;
            if (stm->get()->second == *(cjump->false_label))
                falseLabel = stm->get()->first;
        }
        if (falseLabel){
            for (auto stm = falseLabel->begin(); stm != falseLabel->end(); stm++)
                lastTwo->push_back(stm->get());
            trace(falseLabel);
        }
        else if (trueLabel){
            irt::RelationOperation notOp;
            switch(cjump->rel_op){
              case irt::Eq: notOp = irt::Ne;
              case irt::Ne: notOp = irt::Eq;
              case irt::Lt: notOp = irt::Ge;
              case irt::Gt: notOp = irt::Le;
              case irt::Le: notOp = irt::Gt;
              case irt::Ge: notOp = irt::Lt;
              case irt::Ult: notOp = irt::Uge;
              case irt::Ule: notOp = irt::Ugt;
              case irt::Ugt: notOp = irt::Ule;
              case irt::Uge: notOp = irt::Ult;
            }
            irt::Cjump* headCjump = new irt::Cjump(notOp, std::move(cjump->left),
                                     std::move(cjump->right), cjump->false_label, cjump->true_label);
            trueLabel->push_front(headCjump);
            for (auto stm = trueLabel->begin(); stm != trueLabel->end(); stm++)
                lastTwo->push_back(stm->get());
            trace(trueLabel);
        }
        else {
            temp::Label falseL = temp::Label();
            irt::Label* fLabel = new irt::Label(falseL);
            irt::Cjump* headCjump = new irt::Cjump(cjump->rel_op, std::move(cjump->left),
                                     std::move(cjump->right), cjump->true_label, cjump->false_label);
            StatementList* tail = getNext();
            tail->push_front(fLabel);
            tail->push_front(headCjump);
            for (auto stm = tail->begin(); stm != tail->end(); stm++)
                lastTwo->push_back(stm->get());
        }
    }
    // else error
}

StatementList* Canonizator::getNext(){
    if (!globalBlock.stmLists){
        StatementList* stmList = new StatementList();
        irt::Label* lab = new irt::Label(globalBlock.label);
        stmList->push_back(lab);
        return stmList;
    }
    else {
        StatementList* head = globalBlock.stmLists->front().get();
        auto headhead = dynamic_cast<irt::Label*>(head->front().get());
        auto stmPair = q -> begin();
        while (stmPair != q -> end()){ // see if label exists in the list
            if (stmPair->get()->second == headhead->label){ // can I compare temp::Labels?
                trace(head);
                return head;
            }
            stmPair++;
        }
        globalBlock.stmLists->pop_front();
        return getNext();
    }
}

StatementList* Canonizator::traceSchedule(Block block){
    StatementListList* stmLists;
    globalBlock = block;
    for (stmLists = globalBlock.stmLists; stmLists; stmLists->pop_front()){
        auto head = stmLists->front().get();
        auto headhead = dynamic_cast<irt::Label*>(head->front().get());
        std::pair<StatementList*, temp::Label> pair = std::make_pair(head, headhead->label);
        q->push_back(&pair);
    }
    return getNext();
}
