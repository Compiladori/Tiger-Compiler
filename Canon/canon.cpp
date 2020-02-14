#include "canon.h"

using namespace canon;
using namespace std;
using namespace irt;
bool Canonizator::isNop(Statement* stm) {
  if (auto stmExp_ = dynamic_cast<Exp*>(stm))
    if (dynamic_cast<Const*>(stmExp_->exp.get()))
      return true;
  return false;
}

bool Canonizator::commute(Statement* stm, Expression* exp) {
  if (dynamic_cast<Name*>(exp))
    return true;
  if (dynamic_cast<Const*>(exp))
    return true;
  return isNop(stm);
}

pair<unique_ptr<Statement>, unique_ptr<ExpressionList>> Canonizator::reorder(unique_ptr<ExpressionList> expList) {
  if (expList) {
    cout << "Reorder ";
    expList->print();
    cout << endl;
  }
  if (!expList) {
    return make_pair(make_unique<Exp>(make_unique<Const>(0)), make_unique<ExpressionList>());
  } else if (!expList->size()) {
    return make_pair(make_unique<Exp>(make_unique<Const>(0)), move(expList));
  } else if (auto call = dynamic_cast<Call*>(expList->front().get())) {
    temp::Temp t = temp::Temp();
    unique_ptr<Expression> eseq = make_unique<Eseq>(
        make_unique<Move>(
            make_unique<Temp>(t),
            move(expList->pop_front())),
        make_unique<Temp>(t));
    expList->push_front(move(eseq));
    return reorder(move(expList));
  } else {
    auto head = doExp(expList->pop_front());
    auto tail = reorder(move(expList));
    if (commute(tail.first.get(), head.second.get())) {
      tail.second->push_front(move(head.second));
      return make_pair(sequence(move(head.first), move(tail.first)), move(tail.second));
    } else {
      temp::Temp t = temp::Temp();
      tail.second->push_front(make_unique<Temp>(t));
      auto s = sequence(
          make_unique<Move>(make_unique<Temp>(t), move(head.second)),
          move(tail.first));
      return make_pair(sequence(move(head.first), move(s)), move(tail.second));
    }
  }
}
// Transforms an expression pointer into a unique pointer to that expression
unique_ptr<Expression> Canonizator::makeExpUnique(Expression* exp) {
  unique_ptr<Expression> expU;
  if (auto binop = dynamic_cast<BinOp*>(exp)) {
    expU = make_unique<BinOp>(binop->bin_op, move(binop->left), move(binop->right));
    return expU;
  }
  if (auto mem = dynamic_cast<Mem*>(exp)) {
    expU = make_unique<Mem>(move(mem->exp));
    return expU;
  }
  if (auto temp = dynamic_cast<Temp*>(exp)) {
    expU = make_unique<Temp>(temp->temporary);
    return expU;
  }
  if (auto eseq = dynamic_cast<Eseq*>(exp)) {
    expU = make_unique<Eseq>(move(eseq->stm), move(eseq->exp));
    return expU;
  }
  if (auto name = dynamic_cast<Name*>(exp)) {
    expU = make_unique<Name>(name->name);
    return expU;
  }
  if (auto cte = dynamic_cast<Const*>(exp)) {
    expU = make_unique<Const>(cte->i);
    return expU;
  }
  if (auto call = dynamic_cast<Call*>(exp)) {
    expU = make_unique<Call>(move(call->fun), move(call->args));
    return expU;
  }
}

// Transforms a statement pointer into a unique pointer to that statement
unique_ptr<Statement> Canonizator::makeStmUnique(Statement* stm) {
  unique_ptr<Statement> stmU;
  if (auto seq = dynamic_cast<Seq*>(stm)) {
    stmU = make_unique<Seq>(move(seq->left), move(seq->right));
    return stmU;
  }
  if (auto lab = dynamic_cast<Label*>(stm)) {
    stmU = make_unique<Label>(lab->label);
    return stmU;
  }
  if (auto jmp = dynamic_cast<Jump*>(stm)) {
    stmU = make_unique<Jump>(move(jmp->exp), jmp->label_list);
    return stmU;
  }
  if (auto cjmp = dynamic_cast<Cjump*>(stm)) {
    stmU = make_unique<Cjump>(cjmp->rel_op, move(cjmp->left), move(cjmp->right),
                              move(cjmp->true_label), move(cjmp->false_label));
    return stmU;
  }
  if (auto mov = dynamic_cast<Move*>(stm)) {
    stmU = make_unique<Move>(move(mov->left), move(mov->right));
    return stmU;
  }
  if (auto expr = dynamic_cast<Exp*>(stm)) {
    stmU = make_unique<Exp>(move(expr->exp));
    return stmU;
  }
}

unique_ptr<Statement> Canonizator::sequence(unique_ptr<Statement> stm1, unique_ptr<Statement> stm2) {
  if (isNop(stm1.get()))
    return move(stm2);
  if (isNop(stm2.get()))
    return move(stm1);
  return make_unique<Seq>(move(stm1), move(stm2));
}

// Argument exp will be a call expression
unique_ptr<ExpressionList> Canonizator::getCallRList(unique_ptr<Expression> fun, unique_ptr<ExpressionList> args) {
  auto expList = make_unique<ExpressionList>();
  expList->push_back(move(fun));
  for (int i = 0; i < args->size(); ++i)
    expList->push_back(args->pop_front());
  return expList;
}
unique_ptr<Expression> Canonizator::applyCallRList(unique_ptr<Expression> exp, unique_ptr<ExpressionList> expList) {
  if (auto call = dynamic_cast<Call*>(exp.get())) {
    call->fun = expList->pop_front();
    call->args = move(expList);
  }
  return move(exp);
}

unique_ptr<Statement> Canonizator::doStm(unique_ptr<Statement> stm) {
  cout << "dostm ";
  stm->print();
  cout << endl;
  if (auto seq = dynamic_cast<Seq*>(stm.get())) {
    return sequence(doStm(move(seq->left)), doStm(move(seq->right)));
  }
  if (auto jump = dynamic_cast<Jump*>(stm.get())) {
    auto expList = make_unique<ExpressionList>();
    expList->push_back(move(jump->exp));
    auto result = reorder(move(expList));
    jump->exp = result.second->pop_front();
    return sequence(move(result.first), move(stm));
  }
  if (auto cjump = dynamic_cast<Cjump*>(stm.get())) {
    auto expList = make_unique<ExpressionList>();
    expList->push_back(move(cjump->left));
    expList->push_back(move(cjump->right));
    auto result = reorder(move(expList));
    cjump->left = result.second->pop_front();
    cjump->right = result.second->pop_front();
    return sequence(move(result.first), move(stm));
  }
  if (auto move_stm = dynamic_cast<Move*>(stm.get())) {
    // move->left == dst, move->right == src
    if (auto tmp = dynamic_cast<Temp*>(move_stm->left.get())) {
      if (auto call = dynamic_cast<Call*>(move_stm->right.get())) {
        auto result = reorder(getCallRList(move(call->fun),move(call->args)));
        move_stm -> right = applyCallRList(move(move_stm -> right), move(result.second));
        return sequence(move(result.first), move(stm));
      }
      auto expList = make_unique<ExpressionList>();
      expList->push_back(move(move_stm->right));
      auto result = reorder(move(expList));
      move_stm->right = result.second->pop_front();
      return sequence(move(result.first), move(stm));
    } else if (auto mem = dynamic_cast<Mem*>(move_stm->left.get())) {
      auto expList = make_unique<ExpressionList>();
      expList->push_back(move(move_stm->left));
      auto result = reorder(move(expList));
      move_stm->left = result.second->pop_front();
      return sequence(move(result.first), move(stm));
    } else if (auto eseq = dynamic_cast<Eseq*>(move_stm->left.get())) {
      return doStm(make_unique<Seq>(move(eseq->stm), make_unique<Move>(move(eseq->exp), move(move_stm->right))));
    }
  }
  if (auto expr = dynamic_cast<Exp*>(stm.get())) {
    auto expList = make_unique<ExpressionList>();
    if (auto call = dynamic_cast<Call*>(expr->exp.get())) {
      auto result = reorder(getCallRList(move(call -> fun),move(call -> args)));
      expr->exp = applyCallRList(move(expr->exp), move(result.second));
      return sequence(move(result.first), move(stm));
    }
    expList->push_back(move(expr->exp));
    auto result = reorder(move(expList));
    expr->exp = result.second->pop_front();
    return sequence(move(result.first), move(stm));
  }
  return move(stm);
}

// Given an expression e returns a statement s and an expression e1, where e1 contains
// no ESEQs, such that ESEQ(s, e1) would be equivalent to the original expression e
pair<unique_ptr<Statement>, unique_ptr<Expression>> Canonizator::doExp(unique_ptr<Expression> exp) {
  cout << "doExp ";
  exp->print();
  cout << endl;
  if (auto binop = dynamic_cast<BinOp*>(exp.get())) {
    auto expList = make_unique<ExpressionList>();
    expList->push_back(move(binop->left));
    expList->push_back(move(binop->right));
    auto result = reorder(move(expList));
    binop->right = result.second->pop_front();
    binop->left = result.second->pop_front();
    return make_pair(move(result.first), move(exp));
  }
  if (auto mem = dynamic_cast<Mem*>(exp.get())) {
    auto expList = make_unique<ExpressionList>();
    expList->push_back(move(mem->exp));
    auto result = reorder(move(expList));
    mem->exp = result.second->pop_front();
    return make_pair(move(result.first), move(exp));
  }
  if (auto eseq = dynamic_cast<Eseq*>(exp.get())) {
    auto x = doExp(move(eseq->exp));
    return make_pair(sequence(doStm(move(eseq->stm)), move(x.first)), move(x.second));
  }
  if (auto call = dynamic_cast<Call*>(exp.get())) {
    auto result = reorder(getCallRList(move(call->fun),move(call->args)));
    exp = applyCallRList(move(exp), move(result.second));
    return make_pair(move(result.first), move(exp));
  }
  auto result = reorder(nullptr);
  return make_pair(move(result.first), move(exp));
}

unique_ptr<StatementList> Canonizator::linear(unique_ptr<Statement> stm, unique_ptr<StatementList> right) {
  if (auto seq = dynamic_cast<Seq*>(stm.get()))
    return linear(move(seq->left), linear(move(seq->right), move(right)));
  right->push_front(move(stm));
  return move(right);
}

unique_ptr<StatementList> Canonizator::linearize(unique_ptr<Statement> stm) {
  return linear(doStm(move(stm)), make_unique<StatementList>());
}

StatementListList* Canonizator::createBlocks(StatementList* stmList, temp::Label done) {
  if (!stmList or stmList->size() == 0)
    return nullptr;
  if (auto lab = dynamic_cast<Label*>(stmList->front().get())) {
    StatementList* tail = stmList;
    tail->pop_front();
    StatementListList* res = next(stmList, tail, done);
    res->push_front(stmList);
    return res;
  }
  temp::Label l = temp::Label();
  unique_ptr<Label> lab = make_unique<Label>(l);
  stmList->push_front(move(lab));
  return createBlocks(stmList, done);
}

// looks for a Jump/Cjump or a Label to end or start, respectively, a block in stm
StatementListList* Canonizator::next(StatementList* prevStm, StatementList* stm, temp::Label done) {
  if (!stm or stm->size() == 0) {  // “special” last block – put a JUMP(NAME done) at the end of the last block.
    StatementList* stmListJump = new StatementList();
    temp::LabelList label_list;
    label_list.push_back(done);
    Jump* jump = new Jump(make_unique<Name>(done), label_list);
    stmListJump->push_front(jump);
    return next(prevStm, stmListJump, done);
  }
  if (dynamic_cast<Jump*>(stm->front().get()) or dynamic_cast<Cjump*>(stm->front().get())) {
    StatementListList* stmLists;
    for (auto s = stm->begin(); s != stm->end(); s++)  // push stm elements to the end of prevStm
      prevStm->push_back(s->get());
    //Statement* tmp = prevStm->front().get(); prevStm->pop_front();
    //prevStm = stm; prevStm->push_front(tmp);
    StatementList* tail = stm;
    tail->pop_front();
    stmLists = createBlocks(tail, done);
    Statement* tmp = stm->front().get();
    //if (stm->size() <= 1) return nullptr;
    /*else*/ stm->pop_front();
    stm = nullptr;
    stm->push_front(tmp);
    return stmLists;
  } else if (auto label = dynamic_cast<Label*>(stm->front().get())) {
    temp::Label lab = label->label;
    StatementList* newStm = stm;
    temp::LabelList label_list;
    label_list.push_back(lab);
    Jump* jump = new Jump(make_unique<Name>(lab), label_list);
    newStm->push_front(jump);
    return next(prevStm, newStm, done);
  } else {
    Statement* tmp = prevStm->front().get();
    prevStm->pop_front();
    prevStm = stm;
    prevStm->push_front(tmp);
    StatementList* tail = stm;
    /*if (tail->size() <= 1) tail = nullptr;
      else*/
    tail->pop_front();  // check
    return next(stm, tail, done);
  }
}

struct Block* Canonizator::basicBlocks(StatementList* stmList) {
  temp::Label label = temp::Label();
  StatementListList* stmLists = createBlocks(stmList, label);
  struct Block* block = new Block(stmLists, label);
  return block;
}

void Canonizator::trace(StatementList* stmList) {
  StatementList *lastTwo = stmList, *tmp;
  for (; lastTwo->size() > 2; lastTwo->pop_front())
    ;
  tmp = lastTwo;
  tmp->pop_front();
  Statement* last = tmp->front().get();  // s == last, last == lasttwo
  auto label = dynamic_cast<Label*>(stmList->front().get());
  pair<StatementList*, temp::Label> pair = make_pair((StatementList*)nullptr, label->label);
  q->push_back(&pair);
  if (auto jump = dynamic_cast<Jump*>(last)) {
    StatementList* lab = nullptr;
    temp::LabelList tailLabList(jump->label_list.begin() + 1, jump->label_list.end());
    for (auto stm = q->begin(); stm != q->end(); stm++) {
      if (jump->label_list.front() == stm->get()->second and
          !tailLabList.size()) {
        Statement* head = lastTwo->front().get();
        lastTwo->pop_front();
        lastTwo = stm->get()->first;
        lastTwo->push_front(head);  //remove jump
        trace(stm->get()->first);
      }
    }
    StatementList* tailtail = getNext();
    for (auto stm = tailtail->begin(); stm != tailtail->end(); stm++)
      lastTwo->push_back(stm->get());
  } else if (auto cjump = dynamic_cast<Cjump*>(last)) {
    // look for the true and false label
    StatementList *trueLabel = nullptr, *falseLabel = nullptr;
    for (auto stm = q->begin(); stm != q->end(); stm++) {
      if (stm->get()->second == *(cjump->true_label))
        trueLabel = stm->get()->first;
      if (stm->get()->second == *(cjump->false_label))
        falseLabel = stm->get()->first;
    }
    if (falseLabel) {
      for (auto stm = falseLabel->begin(); stm != falseLabel->end(); stm++)
        lastTwo->push_back(stm->get());
      trace(falseLabel);
    } else if (trueLabel) {
      RelationOperation notOp;
      switch (cjump->rel_op) {
        case Eq:
          notOp = Ne;
        case Ne:
          notOp = Eq;
        case Lt:
          notOp = Ge;
        case Gt:
          notOp = Le;
        case Le:
          notOp = Gt;
        case Ge:
          notOp = Lt;
        case Ult:
          notOp = Uge;
        case Ule:
          notOp = Ugt;
        case Ugt:
          notOp = Ule;
        case Uge:
          notOp = Ult;
      }
      Cjump* headCjump = new Cjump(notOp, move(cjump->left),
                                   move(cjump->right), cjump->false_label, cjump->true_label);
      trueLabel->push_front(headCjump);
      for (auto stm = trueLabel->begin(); stm != trueLabel->end(); stm++)
        lastTwo->push_back(stm->get());
      trace(trueLabel);
    } else {
      temp::Label falseL = temp::Label();
      Label* fLabel = new Label(falseL);
      Cjump* headCjump = new Cjump(cjump->rel_op, move(cjump->left),
                                   move(cjump->right), cjump->true_label, cjump->false_label);
      StatementList* tail = getNext();
      tail->push_front(fLabel);
      tail->push_front(headCjump);
      for (auto stm = tail->begin(); stm != tail->end(); stm++)
        lastTwo->push_back(stm->get());
    }
  }
  // else error
}

StatementList* Canonizator::getNext() {
  if (!globalBlock->stmLists) {
    StatementList* stmList = new StatementList();
    Label* lab = new Label(globalBlock->label);
    stmList->push_back(lab);
    return stmList;
  } else {
    StatementList* head = globalBlock->stmLists->front().get();
    auto headhead = dynamic_cast<Label*>(head->front().get());
    auto stmPair = q->begin();
    while (stmPair != q->end()) {                       // see if label exists in the list
      if (stmPair->get()->second == headhead->label) {  // can I compare temp::Labels?
        trace(head);
        return head;
      }
      stmPair++;
    }
    globalBlock->stmLists->pop_front();
    return getNext();
  }
}

StatementList* Canonizator::traceSchedule(Block* block) {
  StatementListList* stmLists;
  globalBlock = block;
  for (stmLists = globalBlock->stmLists; stmLists; stmLists->pop_front()) {
    auto head = stmLists->front().get();
    auto headhead = dynamic_cast<Label*>(head->front().get());
    pair<StatementList*, temp::Label> pair = make_pair(head, headhead->label);
    q->push_back(&pair);
  }
  return getNext();
}
