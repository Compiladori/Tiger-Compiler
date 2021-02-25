#ifndef __CANON_H__
#define __CANON_H__

#include "../IRT/IRT.h"
#include "../Translation/translatedExpression.h"
#include "../Semantic/environmentTable.h"
#include "../Utility/utility.h"
#include <map>
/***
 * Canonical tree, basic blocks and traces
 *
 * Described in Chapter 8 Appel C (2004)
 * ***/

namespace canon {
using StatementListList = util::GenericList<irt::StatementList>;

struct Block {
    std::unique_ptr<StatementListList> stmLists;
    temp::Label label;
    virtual ~Block() {}

    Block(std::unique_ptr<StatementListList> stmLists, temp::Label label) : stmLists(move(stmLists)), label(label) {}
};

class Canonizator {
    std::map<temp::Label, irt::StatementList*> basic_blocks_table;
    bool isNop(irt::Statement* stm);
    bool commute(irt::Statement* stm, irt::Expression* exp);
    std::pair<std::unique_ptr<irt::Statement>, std::unique_ptr<irt::ExpressionList>> reorder(std::unique_ptr<irt::ExpressionList> expList);    std::unique_ptr<irt::Statement> doStm(std::unique_ptr<irt::Statement> stm);
    std::pair<std::unique_ptr<irt::Statement>, std::unique_ptr<irt::Expression>> doExp(std::unique_ptr<irt::Expression> exp);
    std::unique_ptr<irt::Statement> sequence(std::unique_ptr<irt::Statement> stm1,std::unique_ptr<irt::Statement> stm2);
    std::unique_ptr<irt::ExpressionList> getCallRList(std::unique_ptr<irt::Expression> fun,std::unique_ptr<irt::ExpressionList> args);
    std::unique_ptr<irt::Expression> applyCallRList(std::unique_ptr<irt::Expression> exp,std::unique_ptr<irt::ExpressionList> expList);
    std::unique_ptr<irt::StatementList> linear(std::unique_ptr<irt::Statement> stm, std::unique_ptr<irt::StatementList> right);
    std::unique_ptr<StatementListList> createBlocks(std::unique_ptr<irt::StatementList> stmList, temp::Label label,std::unique_ptr<StatementListList> res);
    std::unique_ptr<StatementListList> next(std::unique_ptr<irt::StatementList> prevStm, std::unique_ptr<StatementListList> stm, temp::Label done);
    std::unique_ptr<irt::StatementList> getNext(std::unique_ptr<Block> block,std::unique_ptr<irt::StatementList> res);
    std::unique_ptr<irt::StatementList> trace(std::unique_ptr<Block> block,std::unique_ptr<irt::StatementList> res,temp::Label lbl);
public:
    // Canonizator() = default;
    std::unique_ptr<irt::StatementList> linearize(std::unique_ptr<irt::Statement> stm);
    std::unique_ptr<Block> basicBlocks(std::unique_ptr<irt::StatementList> stmList);
    std::unique_ptr<irt::StatementList> traceSchedule(std::unique_ptr<Block> block);
};


};

#endif
