#ifndef __CANON_H__
#define __CANON_H__

#include "../IRT/IRT.h"
#include "../Translation/translatedExpression.h"
#include "../Semantic/environmentTable.h"

/***
 * Canonical tree, basic blocks and traces
 *
 * Described in Chapter 8 Appel C (2004)
 * ***/

namespace canon {
using StatementListList = util::GenericList<irt::StatementList>;
using StmListLabel = util::GenericList<std::pair<irt::StatementList*, temp::Label>>;

struct Block {
    StatementListList* stmLists;
    temp::Label label;
    virtual ~Block() {}

    Block(StatementListList* stmLists, temp::Label label) : stmLists(stmLists), label(label) {}
};

struct StmExpList {
    std::unique_ptr<irt::Statement> stm;
    std::unique_ptr<irt::ExpressionList> expList;
    virtual ~StmExpList() {}

    StmExpList(irt::Statement* stm, irt::ExpressionList* expList) : stm(stm), expList(expList) {}
};

struct Canonizator {
    StmListLabel*  q;
    Block* globalBlock;
    bool isNop(irt::Statement* stm);
    bool commute(irt::Statement* stm, irt::Expression* exp);
    std::pair<std::unique_ptr<irt::Statement>, std::unique_ptr<irt::ExpressionList>> reorder(std::unique_ptr<irt::ExpressionList> expList);    std::unique_ptr<irt::Statement> doStm(std::unique_ptr<irt::Statement> stm);
    std::pair<std::unique_ptr<irt::Statement>, std::unique_ptr<irt::Expression>> doExp(std::unique_ptr<irt::Expression> exp);
    std::unique_ptr<irt::Statement> sequence(std::unique_ptr<irt::Statement> stm1,std::unique_ptr<irt::Statement> stm2);
    std::unique_ptr<irt::Expression> makeExpUnique(irt::Expression* exp);
    std::unique_ptr<irt::Statement> makeStmUnique(irt::Statement* stm);
    std::unique_ptr<irt::ExpressionList> getCallRList(std::unique_ptr<irt::Expression> fun,std::unique_ptr<irt::ExpressionList> args);
    std::unique_ptr<irt::Expression> applyCallRList(std::unique_ptr<irt::Expression> exp,std::unique_ptr<irt::ExpressionList> expList);
    std::unique_ptr<irt::StatementList> linear(std::unique_ptr<irt::Statement> stm, std::unique_ptr<irt::StatementList> right);
    StatementListList* createBlocks(irt::StatementList* stmList, temp::Label label);
    StatementListList* next(irt::StatementList* prevStm, irt::StatementList* stm, temp::Label done);
    irt::StatementList* getNext();
    void trace(irt::StatementList* stmList);
public:
    Canonizator() : q(nullptr), globalBlock(nullptr) {};
    std::unique_ptr<irt::StatementList> linearize(std::unique_ptr<irt::Statement> stm);
    struct Block* basicBlocks(irt::StatementList* stmList);
    irt::StatementList* traceSchedule(Block* block);
};


};

#endif
