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

struct Canonizator {
    Block block;
    StatementList* stmList;
    ExpressionList* expList;
    auto linearize(irt::Statement* stm);

    auto basicBlocks(StatementList* stmList);

    StatementList* traceSchedule(Block block);

    bool isNop(irt::Statement* stm);
    bool commute(irt::Statement* stm, irt::Expression* exp);
public:
    Canonizator();

};


};

#endif
