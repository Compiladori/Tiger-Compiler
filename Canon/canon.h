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
//using StatementListList = util::GenericList<StatementList>;

struct Block {
    virtual ~Block() {}
    virtual void print() const = 0;
};

struct BasicBlocks : public Block {
    std::unique_ptr<StatementList> stmList;

    BasicBlocks(std::unique_ptr<StatementList> stmList) : stmList(std::move(stmList)) {}

    StatementList* traceSchedule();
    void print() const;
};

};

#endif
