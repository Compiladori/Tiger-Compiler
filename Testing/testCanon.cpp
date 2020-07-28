#include "catch2/catch.hpp"

#include "../Canon/canon.h"

using namespace std;

TEST_CASE("linearize", "[canon]") {
  canon::Canonizator c;
  SECTION("just a Label"){
    // must return [label]
    unique_ptr<irt::Statement> stm = make_unique<irt::Label>(temp::Label());
    unique_ptr<irt::StatementList> stmList = c.linearize(move(stm));
    REQUIRE(stmList->size() == 1);
    REQUIRE(dynamic_cast<irt::Label*>(stmList->front().get()));
  }

  SECTION("ESEQ(s1, ESEQ(s2, e))"){ // s1 : Label, s2 : Label, e : Const
    // must return [s1, s2]
    unique_ptr<irt::Label> s1 = make_unique<irt::Label>(temp::Label());
    unique_ptr<irt::Label> s2 = make_unique<irt::Label>(temp::Label());
    unique_ptr<irt::Const> ex = make_unique<irt::Const>(0);
    unique_ptr<irt::Eseq> e = make_unique<irt::Eseq>(move(s2), move(ex));
    unique_ptr<irt::Eseq> eseq = make_unique<irt::Eseq>(move(s1), move(e));
    unique_ptr<irt::Statement> exp = make_unique<irt::Exp>(move(eseq));
    unique_ptr<irt::StatementList> stmList = c.linearize(move(exp));
    REQUIRE(stmList->size() == 2);
    REQUIRE(dynamic_cast<irt::Label*>(stmList->front().get()));
    REQUIRE(dynamic_cast<irt::Label*>(stmList->back().get()));
  }

  SECTION("BINOP(op, ESEQ(s, e1), e2)"){ // s : Label, e1 : Temp, e2 : Temp
    // must return [s, Exp (BinOp (op, e1, e2))]
    unique_ptr<irt::Label> s = make_unique<irt::Label>(temp::Label());
    unique_ptr<irt::Temp> e = make_unique<irt::Temp>(temp::Temp());
    unique_ptr<irt::Eseq> exp1 = make_unique<irt::Eseq>(move(s), move(e));
    unique_ptr<irt::Temp> exp2 = make_unique<irt::Temp>(temp::Temp());
    unique_ptr<irt::BinOp> binop = make_unique<irt::BinOp>(irt::Plus, move(exp1), move(exp2));
    unique_ptr<irt::Statement> exp = make_unique<irt::Exp>(move(binop));
    unique_ptr<irt::StatementList> stmList = c.linearize(move(exp));
    REQUIRE(stmList->size() == 2);
    REQUIRE(dynamic_cast<irt::Label*>(stmList->front().get()));
    REQUIRE(dynamic_cast<irt::Exp*>(stmList->back().get()));
    irt::Exp* tmp = dynamic_cast<irt::Exp*>(stmList->back().get());
    REQUIRE(dynamic_cast<irt::BinOp*>(tmp->exp.get()));
  }

  SECTION("MEM(ESEQ(s, e1))"){ // s : Label, e1 : Const
    // must return [s, Mem(e1)]
    unique_ptr<irt::Label> s = make_unique<irt::Label>(temp::Label());
    unique_ptr<irt::Const> e1 = make_unique<irt::Const>(1);
    unique_ptr<irt::Eseq> eseq = make_unique<irt::Eseq>(move(s), move(e1));
    unique_ptr<irt::Mem> mem = make_unique<irt::Mem>(move(eseq));
    unique_ptr<irt::Statement> exp = make_unique<irt::Exp>(move(mem));
    unique_ptr<irt::StatementList> stmList = c.linearize(move(exp));
    REQUIRE(stmList->size() == 2);
    REQUIRE(dynamic_cast<irt::Label*>(stmList->front().get()));
    REQUIRE(dynamic_cast<irt::Exp*>(stmList->back().get()));
    irt::Exp* tmp = dynamic_cast<irt::Exp*>(stmList->back().get());
    REQUIRE(dynamic_cast<irt::Mem*>(tmp->exp.get()));
  }

}

TEST_CASE("basicBlocks", "[canon]") {
  canon::Canonizator c;
  SECTION("empty list"){
    // must return []
    unique_ptr<irt::StatementList> stmList = make_unique<irt::StatementList>();
    unique_ptr<canon::Block> b = c.basicBlocks(move(stmList));
    REQUIRE(b->stmLists->size() == 0);
  }

  SECTION("just a Label"){
    // must return [[Label, Jump]]
    unique_ptr<irt::StatementList> stmList = make_unique<irt::StatementList>();
    unique_ptr<irt::Label> label = make_unique<irt::Label>(temp::Label());
    stmList->push_back(move(label));
    unique_ptr<canon::Block> b = c.basicBlocks(move(stmList));
    REQUIRE(b->stmLists->size() == 1);
    REQUIRE(b->stmLists->front()->size() == 2);
    REQUIRE(dynamic_cast<irt::Label*>(b->stmLists->front()->front().get()));
    REQUIRE(dynamic_cast<irt::Jump*>(b->stmLists->front()->back().get()));
  }

  SECTION("just a Jump"){
    // must return [[Label, Jump]]
    unique_ptr<irt::StatementList> stmList = make_unique<irt::StatementList>();
    temp::Label lab = temp::Label();
    unique_ptr<irt::Name> exp = make_unique<irt::Name>(lab);
    temp::LabelList label_list = temp::LabelList(1, lab);
    unique_ptr<irt::Jump> jump = make_unique<irt::Jump>(move(exp), label_list);
    stmList->push_back(move(jump));
    unique_ptr<canon::Block> b = c.basicBlocks(move(stmList));
    REQUIRE(b->stmLists->size() == 1);
    REQUIRE(b->stmLists->front()->size() == 2);
    REQUIRE(dynamic_cast<irt::Label*>(b->stmLists->front()->front().get()));
    REQUIRE(dynamic_cast<irt::Jump*>(b->stmLists->front()->back().get()));
  }

  SECTION("[Label, Exp (BinOp (Plus, Const(0), Const(1))), Jump]"){
    // must return [[Label, Exp (BinOp (Plus, Const(0), Const(1))), Jump]]
    unique_ptr<irt::StatementList> stmList = make_unique<irt::StatementList>();
    unique_ptr<irt::Label> label = make_unique<irt::Label>(temp::Label());
    stmList->push_back(move(label));
    unique_ptr<irt::Const> exp1 = make_unique<irt::Const>(0);
    unique_ptr<irt::Const> exp2 = make_unique<irt::Const>(1);
    unique_ptr<irt::BinOp> binop = make_unique<irt::BinOp>(irt::Plus, move(exp1), move(exp2));
    unique_ptr<irt::Exp> e = make_unique<irt::Exp>(move(binop));
    stmList->push_back(move(e));
    temp::Label lab = temp::Label();
    unique_ptr<irt::Name> exp = make_unique<irt::Name>(lab);
    temp::LabelList label_list = temp::LabelList(1, lab);
    unique_ptr<irt::Jump> jump = make_unique<irt::Jump>(move(exp), label_list);
    stmList->push_back(move(jump));
    unique_ptr<canon::Block> b = c.basicBlocks(move(stmList));
    REQUIRE(b->stmLists->size() == 1);
    REQUIRE(b->stmLists->front()->size() == 3);
    REQUIRE(dynamic_cast<irt::Label*>(b->stmLists->front()->front().get()));
    REQUIRE(dynamic_cast<irt::Jump*>(b->stmLists->front()->back().get()));
    b->stmLists->front()->pop_front();
    REQUIRE(dynamic_cast<irt::Exp*>(b->stmLists->front()->front().get()));
    irt::Exp* tmp = dynamic_cast<irt::Exp*>(b->stmLists->front()->front().get());
    REQUIRE(dynamic_cast<irt::BinOp*>(tmp->exp.get()));
  }

  SECTION("[Label, Exp (BinOp (Plus, Const(0), Const(1))), Label, Exp (Mem (Const(100)))]"){
    // must return [[Label, Exp (BinOp (Plus, Const(0), Const(1))), Jump],
    //              [Label, Exp (Mem (Const(100))), Jump]]
    unique_ptr<irt::StatementList> stmList = make_unique<irt::StatementList>();
    unique_ptr<irt::Label> label1 = make_unique<irt::Label>(temp::Label());
    stmList->push_back(move(label1));
    unique_ptr<irt::Const> exp1 = make_unique<irt::Const>(0);
    unique_ptr<irt::Const> exp2 = make_unique<irt::Const>(1);
    unique_ptr<irt::BinOp> binop = make_unique<irt::BinOp>(irt::Plus, move(exp1), move(exp2));
    unique_ptr<irt::Exp> e1 = make_unique<irt::Exp>(move(binop));
    stmList->push_back(move(e1));
    unique_ptr<irt::Label> label2 = make_unique<irt::Label>(temp::Label());
    stmList->push_back(move(label2));
    unique_ptr<irt::Const> exp3 = make_unique<irt::Const>(100);
    unique_ptr<irt::Mem> mem = make_unique<irt::Mem>(move(exp3));
    unique_ptr<irt::Exp> e2 = make_unique<irt::Exp>(move(mem));
    stmList->push_back(move(e2));
    unique_ptr<canon::Block> b = c.basicBlocks(move(stmList));
    REQUIRE(b->stmLists->size() == 2);
    REQUIRE(b->stmLists->front()->size() == 3);
    REQUIRE(b->stmLists->back()->size() == 3);
    REQUIRE(dynamic_cast<irt::Label*>(b->stmLists->front()->front().get()));
    REQUIRE(dynamic_cast<irt::Jump*>(b->stmLists->front()->back().get()));
    b->stmLists->front()->pop_front();
    REQUIRE(dynamic_cast<irt::Exp*>(b->stmLists->front()->front().get()));
    irt::Exp* tmp = dynamic_cast<irt::Exp*>(b->stmLists->front()->front().get());
    REQUIRE(dynamic_cast<irt::BinOp*>(tmp->exp.get()));
    REQUIRE(dynamic_cast<irt::Label*>(b->stmLists->back()->front().get()));
    REQUIRE(dynamic_cast<irt::Jump*>(b->stmLists->back()->back().get()));
    b->stmLists->back()->pop_front();
    REQUIRE(dynamic_cast<irt::Exp*>(b->stmLists->back()->front().get()));
    tmp = dynamic_cast<irt::Exp*>(b->stmLists->back()->front().get());
    REQUIRE(dynamic_cast<irt::Mem*>(tmp->exp.get()));
  }

  SECTION("[Exp (Mem (Const(100)))]"){
    // must return [Label, Exp (Mem (Const(100))), Jump]
    unique_ptr<irt::StatementList> stmList = make_unique<irt::StatementList>();
    unique_ptr<irt::Const> exp = make_unique<irt::Const>(100);
    unique_ptr<irt::Mem> mem = make_unique<irt::Mem>(move(exp));
    unique_ptr<irt::Exp> e = make_unique<irt::Exp>(move(mem));
    stmList->push_back(move(e));
    unique_ptr<canon::Block> b = c.basicBlocks(move(stmList));
    REQUIRE(b->stmLists->size() == 1);
    REQUIRE(b->stmLists->front()->size() == 3);
    REQUIRE(dynamic_cast<irt::Label*>(b->stmLists->front()->front().get()));
    REQUIRE(dynamic_cast<irt::Jump*>(b->stmLists->front()->back().get()));
    b->stmLists->front()->pop_front();
    REQUIRE(dynamic_cast<irt::Exp*>(b->stmLists->front()->front().get()));
    irt::Exp* tmp = dynamic_cast<irt::Exp*>(b->stmLists->front()->front().get());
    REQUIRE(dynamic_cast<irt::Mem*>(tmp->exp.get()));
  }
}

TEST_CASE("traceSchedule", "[canon]") {
  canon::Canonizator c;
  unique_ptr<canon::StatementListList> stmLists = make_unique<canon::StatementListList>();
  unique_ptr<irt::StatementList> stmList1 = make_unique<irt::StatementList>();
  unique_ptr<irt::Label> label1 = make_unique<irt::Label>(temp::Label());
  stmList1->push_back(move(label1));
  temp::Label label2 = temp::Label();
  unique_ptr<irt::Name> exp = make_unique<irt::Name>(label2);
  temp::LabelList label_list = temp::LabelList(1, label2);
  unique_ptr<irt::Jump> jump1 = make_unique<irt::Jump>(move(exp), label_list);
  temp::Label label = temp::Label();

  SECTION("[[Label1, Jump1]]"){
    stmList1->push_back(move(jump1));
    stmLists->push_back(move(stmList1));
    unique_ptr<canon::Block> b = make_unique<canon::Block>(move(stmLists), label);
    unique_ptr<irt::StatementList> res = c.traceSchedule(move(b));
    REQUIRE(res->size() == 2);
    REQUIRE(dynamic_cast<irt::Label*>(res->front().get()));
    REQUIRE(dynamic_cast<irt::Label*>(res->back().get()));
  }

  unique_ptr<irt::Const> expr1 = make_unique<irt::Const>(0);
  unique_ptr<irt::Const> expr2 = make_unique<irt::Const>(1);
  unique_ptr<irt::BinOp> binop = make_unique<irt::BinOp>(irt::Plus, move(expr1), move(expr2));
  unique_ptr<irt::Exp> e1 = make_unique<irt::Exp>(move(binop));
  unique_ptr<irt::StatementList> stmList2 = make_unique<irt::StatementList>();
  unique_ptr<irt::Label> label3 = make_unique<irt::Label>(temp::Label());

  SECTION("[[Label1, Jump2], [Label3, Exp (BinOp (Plus, Const(0), Const(1))), Jump3]]"){
    unique_ptr<irt::Name> exp1 = make_unique<irt::Name>(label3->label);
    temp::LabelList label_list1 = temp::LabelList(1, label3->label);
    stmList2->push_back(move(label3));
    unique_ptr<irt::Jump> jump2 = make_unique<irt::Jump>(move(exp1), label_list1);
    stmList1->push_back(move(jump2));
    stmLists->push_back(move(stmList1));

    stmList2->push_back(move(e1));
    temp::Label label4 = temp::Label();
    unique_ptr<irt::Name> exp2 = make_unique<irt::Name>(label4);
    temp::LabelList label_list2 = temp::LabelList(1, label4);
    unique_ptr<irt::Jump> jump3 = make_unique<irt::Jump>(move(exp2), label_list2);
    stmList2->push_back(move(jump3));
    stmLists->push_back(move(stmList2));

    unique_ptr<canon::Block> b = make_unique<canon::Block>(move(stmLists), label);
    unique_ptr<irt::StatementList> res = c.traceSchedule(move(b));
    REQUIRE(res->size() == 4); // [Label, Label, Exp (BinOp (...)), Label]
    REQUIRE(dynamic_cast<irt::Label*>(res->front().get()));
    REQUIRE(dynamic_cast<irt::Label*>(res->back().get()));
    res->pop_front();
    REQUIRE(dynamic_cast<irt::Label*>(res->front().get()));
    res->pop_back();
    REQUIRE(dynamic_cast<irt::Exp*>(res->back().get()));
  }

  SECTION("[[Label1, Exp (Mem ()), CJump1], [Label3, Exp (BinOp ()), Jump]], [Label4, Move (), Jump]"){
    // must return [Label1, Exp (Mem ()), CJump1, Label4, Move (), Label done]
    unique_ptr<irt::Const> expr3 = make_unique<irt::Const>(100);
    unique_ptr<irt::Mem> mem = make_unique<irt::Mem>(move(expr3));
    unique_ptr<irt::Exp> e2 = make_unique<irt::Exp>(move(mem));
    stmList1->push_back(move(e2));
    unique_ptr<irt::Label> label4 = make_unique<irt::Label>(temp::Label());
    unique_ptr<irt::Const> left = make_unique<irt::Const>(20);
    unique_ptr<irt::Const> right = make_unique<irt::Const>(10);
    unique_ptr<irt::Cjump> cjump1 = make_unique<irt::Cjump>(irt::Lt, move(left), move(right), label3->label, label4->label);
    stmList1->push_back(move(cjump1));
    stmLists->push_back(move(stmList1));

    stmList2->push_back(move(label3));
    stmList2->push_back(move(e1));
    temp::Label label5 = temp::Label();
    unique_ptr<irt::Name> e3 = make_unique<irt::Name>(label5);
    temp::LabelList label_list1 = temp::LabelList(1, label5);
    unique_ptr<irt::Jump> jump2 = make_unique<irt::Jump>(move(e3), label_list1);
    stmList2->push_back(move(jump2));
    stmLists->push_back(move(stmList2));

    unique_ptr<irt::StatementList> stmList3 = make_unique<irt::StatementList>();
    stmList3->push_back(move(label4));
    unique_ptr<irt::Temp> tmp = make_unique<irt::Temp>(temp::Temp());
    unique_ptr<irt::Const> cte = make_unique<irt::Const>(2);
    unique_ptr<irt::Move> mov = make_unique<irt::Move>(move(tmp), move(cte));
    stmList3->push_back(move(mov));
    temp::Label label6 = temp::Label();
    unique_ptr<irt::Name> e4 = make_unique<irt::Name>(label6);
    temp::LabelList label_list2 = temp::LabelList(1, label6);
    unique_ptr<irt::Jump> jump3 = make_unique<irt::Jump>(move(e4), label_list2);
    stmList3->push_back(move(jump3));
    stmLists->push_back(move(stmList3));
    unique_ptr<canon::Block> b = make_unique<canon::Block>(move(stmLists), label);
    unique_ptr<irt::StatementList> res = c.traceSchedule(move(b));
    REQUIRE(res->size() == 6);
    REQUIRE(dynamic_cast<irt::Label*>(res->front().get()));
    REQUIRE(dynamic_cast<irt::Label*>(res->back().get()));
    res->pop_front();
    REQUIRE(dynamic_cast<irt::Exp*>(res->front().get()));
    res->pop_front();
    REQUIRE(dynamic_cast<irt::Cjump*>(res->front().get()));
    res->pop_front();
    REQUIRE(dynamic_cast<irt::Label*>(res->front().get()));
    res->pop_front();
    REQUIRE(dynamic_cast<irt::Move*>(res->front().get()));
  }
}


// $ g++ testMain.cpp -c
// $ g++ ../Canon/canon.cpp -c
// $ g++ testMain.o canon.o (...) testCanon.cpp -o tests && ./tests -r compact
