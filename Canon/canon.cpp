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
        auto result = reorder(getCallRList(move(call->fun), move(call->args)));
        move_stm->right = applyCallRList(move(move_stm->right), move(result.second));
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
      auto result = reorder(getCallRList(move(call->fun), move(call->args)));
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
    auto result = reorder(getCallRList(move(call->fun), move(call->args)));
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

unique_ptr<StatementListList> Canonizator::createBlocks(unique_ptr<StatementList> stmList, temp::Label done, unique_ptr<StatementListList> res) {
  if (!stmList or stmList->size() == 0)
    return move(res);
  if (auto lab = dynamic_cast<Label*>(stmList->front().get())) {
    auto label = stmList->pop_front();
    auto label_list = make_unique<StatementList>();
    label_list->push_front(move(label));
    res->push_back(move(label_list));
    return next(move(stmList), move(res), done);
  }
  stmList->push_front(make_unique<Label>(temp::Label()));
  return createBlocks(move(stmList), done, move(res));
}

// looks for a Jump/Cjump or a Label to end or start, respectively, a block in stm
unique_ptr<StatementListList> Canonizator::next(unique_ptr<StatementList> stmList, unique_ptr<StatementListList> res, temp::Label done) {
  if (!stmList or stmList->size() == 0) {  // “special” last block – put a JUMP(NAME done) at the end of the last block.
    temp::LabelList label_list;
    label_list.push_back(done);
    auto jump = make_unique<Jump>(make_unique<Name>(done), label_list);
    stmList->push_front(move(jump));
    return next(move(stmList), move(res), done);
  }
  if (dynamic_cast<Jump*>(stmList->front().get()) or dynamic_cast<Cjump*>(stmList->front().get())) {
    res->back()->push_back(stmList->pop_front());
    return createBlocks(move(stmList), done, move(res));
  } else if (auto label = dynamic_cast<Label*>(stmList->front().get())) {
    temp::Label lab = label->label;
    temp::LabelList label_list;
    label_list.push_back(lab);
    auto jump = make_unique<Jump>(make_unique<Name>(lab), label_list);
    stmList->push_front(move(jump));
    return next(move(stmList), move(res), done);
  } else {
    res->back()->push_back(stmList->pop_front());
    return next(move(stmList), move(res), done);
  }
}

unique_ptr<Block> Canonizator::basicBlocks(unique_ptr<StatementList> stmList) {
  temp::Label label = temp::Label();
  auto stmLists = createBlocks(move(stmList), label, make_unique<StatementListList>());
  return make_unique<Block>(move(stmLists), label);
}

RelationOperation NegateRelOp(RelationOperation r) {
  switch (r) {
    case Eq:
      return Ne;
    case Ne:
      return Eq;
    case Lt:
      return Ge;
    case Ge:
      return Lt;
    case Gt:
      return Le;
    case Le:
      return Gt;
  }
  exit(-1);
}

unique_ptr<StatementList> Canonizator::getNext(unique_ptr<Block> block, unique_ptr<StatementList> res, temp::Label label) {
  if (!block->stmLists or !block->stmLists->size()) {
    res->push_back(make_unique<Label>(block->label));
    return move(res);
  }
  if (!block->stmLists->front()->size()) {
    block->stmLists->pop_front();
    return getNext(move(block), move(res), label);
  }
  if (basic_blocks_table.count(label)) {
    auto stm_ptr = basic_blocks_table[label];
    basic_blocks_table.erase(label);
    for (auto const& stm : *stm_ptr) {
      res->push_back(stm_ptr->pop_front());
    }
    block->stmLists->pop_front();
    if (auto last_stm = dynamic_cast<irt::Jump*>(res->back().get())) {
      if (last_stm->label_list.size() == 1)
        res->pop_back();
    } else if (auto last_stm = dynamic_cast<irt::Cjump*>(res->back().get())) {
      if (basic_blocks_table.count(last_stm->false_label)) {
        return getNext(move(block), move(res), last_stm->false_label);
      }
      if (basic_blocks_table.count(last_stm->true_label)) {
        res->pop_back();
        res->push_back(make_unique<Cjump>(NegateRelOp(last_stm->rel_op), move(last_stm->left), move(last_stm->right), last_stm->false_label, last_stm->true_label));
        return getNext(move(block), move(res), last_stm->true_label);
      }
      res->pop_back();
      auto new_label = temp::Label();
      res->push_back(make_unique<irt::Cjump>(last_stm->rel_op, move(last_stm->left), move(last_stm->right), last_stm->true_label, new_label));
      res->push_back(make_unique<irt::Label>(new_label));
    }
    return getNext(move(block), move(res), label);
  } else {
    for (const auto& p : *block->stmLists) {
      if (!block->stmLists->front()->size())
        block->stmLists->pop_front();
      else {
        if (auto lbl_stm = dynamic_cast<irt::Label*>(block->stmLists->front()->front().get()))
          return getNext(move(block), move(res), lbl_stm->label);
        else
          //should be a label
          exit(-1);
      }
    }
  }
}

unique_ptr<StatementList> Canonizator::traceSchedule(unique_ptr<Block> block) {
  for (const auto& p : *block->stmLists) {
    if (auto label = dynamic_cast<irt::Label*>(p->front().get())) {
      if (not basic_blocks_table.count(label->label))
        basic_blocks_table[label->label] = p.get();
      else
        // this means that the first element of a list isn't an unique id, big issue
        exit(-1);
    } else {
      // this means that the first element of a list isn't a label, big issue
      exit(-1);
    }
  }
  auto label = dynamic_cast<irt::Label*>(block->stmLists->front()->front().get());
  return getNext(move(block), make_unique<StatementList>(), label->label);
}
