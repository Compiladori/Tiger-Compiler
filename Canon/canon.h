#ifndef __CANON_H__
#define __CANON_H__

#include "../IRT/IRT.h"
#include "../Translation/translatedExpression.h"

/***
 * Canonical tree, basic blocks and traces
 *
 * Described in Chapter 8 Appel C (2004)
 * ***/

namespace canon {

using StatementList = util::GenericList<irt::Statement>;
using ExpressionList = util::GenericList<irt::Expression>;
using StatementListList = util::GenericList<StatementList>;

struct Block {
    StatementListList* stmLists;
    temp::Label label;
    virtual ~Block() {}

    Block(StatementListList* stmLists, temp::Label label) : stmLists(stmLists), label(label) {}

};

struct StmExpList {
    std::unique_ptr<irt::Statement> stm;
    std::unique_ptr<ExpressionList> expList;
    virtual ~StmExpList() {}

    StmExpList(irt::Statement* stm, irt::ExpressionList* expList) : stm(stm), expList(expList) {}
};

struct Canonizator {
    //Block block;
    //StatementList* stmList;
    //ExpressionList* expList;

    bool isNop(irt::Statement* stm);
    bool commute(irt::Statement* stm, irt::Expression* exp);
    irt::Statement* reorder(ExpressionList* expList);
    // doExp returns a statement and an expression (list of one expression)
    irt::Statement* doStm(irt::Statement* stm);
    std::pair<irt::Statement*, irt::Expression*> doExp(irt::Expression* exp);
    irt::Statement* sequence(irt::Statement* stm1, irt::Statement* stm2);
    std::unique_ptr<irt::Expression> makeExpUnique(irt::Expression* exp);
    std::unique_ptr<irt::Statement> makeStmUnique(irt::Statement* stm);
    ExpressionList* getCallRList(irt::Expression* exp);
    StatementList* linear(irt::Statement* stm, StatementList* right);
public:
    Canonizator();
    // ............... ver tipos!!!!!
    StatementList* linearize(irt::Statement* stm);
    struct Block* basicBlocks(StatementList* stmList);
    StatementList* traceSchedule(Block block);
};


};

#endif
