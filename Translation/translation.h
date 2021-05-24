#ifndef __TRANSLATION_H__
#define __TRANSLATION_H__

/**
 * Translation module from Abstract Syntax into IRT
 *
 * Described in Chapter 7 Appel C (2004)
 * **/

#include "../Frame/frame.h"
#include "../Frame/temp.h"
#include "../Semantic/expressionType.h"
#include "../Translation/translatedExpression.h"
#include "../Utility/utility.h"
namespace trans {
class Access;
class Level;
using AccessList = std::vector<std::shared_ptr<Access>>;

/**
 * Main translating class
 * **/
class Translator {
    // Stuff

   public:
    std::unique_ptr<frame::FragList> _frag_list;
    Translator() { _frag_list = std::make_unique<frame::FragList>(); }
    /**
     * Variable translation
     * **/
    std::unique_ptr<TranslatedExp> simpleVar(std::shared_ptr<trans::Access> a, std::shared_ptr<trans::Level>);
    std::unique_ptr<TranslatedExp> fieldVar(std::unique_ptr<TranslatedExp> var, int fieldIndex);
    std::unique_ptr<TranslatedExp> subscriptVar(std::unique_ptr<TranslatedExp> var, std::unique_ptr<TranslatedExp> sub);

    /**
     * Expression translation
     * **/
    std::unique_ptr<TranslatedExp> unitExp();
    std::unique_ptr<TranslatedExp> nilExp();
    std::unique_ptr<TranslatedExp> intExp(ast::IntExp* val);
    std::unique_ptr<TranslatedExp> stringExp(ast::StringExp* str);
    std::unique_ptr<TranslatedExp> callExp(bool isLibFunc, std::shared_ptr<trans::Level> funlvl, std::shared_ptr<trans::Level> currentlvl, temp::Label name, std::unique_ptr<ExpressionList> list);
    std::unique_ptr<TranslatedExp> arExp(ast::Operation op, std::unique_ptr<TranslatedExp> exp1, std::unique_ptr<TranslatedExp> exp2);
    std::unique_ptr<TranslatedExp> condExp(ast::Operation op, std::unique_ptr<TranslatedExp> exp1, std::unique_ptr<TranslatedExp> exp2);
    std::unique_ptr<TranslatedExp> strExp(ast::Operation op, std::unique_ptr<TranslatedExp> exp1, std::unique_ptr<TranslatedExp> exp2);
    std::unique_ptr<TranslatedExp> recordExp(std::unique_ptr<trans::ExpressionList> el, int fieldCount);
    std::unique_ptr<TranslatedExp> seqExp(std::unique_ptr<trans::ExpressionList> list);
    std::unique_ptr<TranslatedExp> assignExp(std::unique_ptr<TranslatedExp> var, std::unique_ptr<TranslatedExp> exp);
    std::unique_ptr<TranslatedExp> ifExp(std::unique_ptr<TranslatedExp> test, std::unique_ptr<TranslatedExp> then, std::unique_ptr<TranslatedExp> elsee, seman::ExpType* if_type);
    std::unique_ptr<TranslatedExp> whileExp(std::unique_ptr<TranslatedExp> exp, std::unique_ptr<TranslatedExp> body, temp::Label breaklbl);
    std::unique_ptr<TranslatedExp> forExp(std::shared_ptr<trans::Access> access, std::shared_ptr<trans::Level> lvl, std::unique_ptr<TranslatedExp> explo, std::unique_ptr<TranslatedExp> exphi, std::unique_ptr<TranslatedExp> body, temp::Label breaklbl);
    std::unique_ptr<TranslatedExp> letExp(std::unique_ptr<ExpressionList> list, std::unique_ptr<TranslatedExp> body);
    std::unique_ptr<TranslatedExp> breakExp(temp::Label breaklbl);
    std::unique_ptr<TranslatedExp> arrayExp(std::unique_ptr<TranslatedExp> init, std::unique_ptr<TranslatedExp> size);
    std::unique_ptr<frame::FragList> procEntryExit(std::shared_ptr<Level> lvl, std::unique_ptr<TranslatedExp> body, std::shared_ptr<AccessList> formals);
    /**
     * Utility Functions
     * **/
    static std::unique_ptr<TranslatedExp> nullNx();
    static std::unique_ptr<TranslatedExp> NoExp();
    void proc_entry_exit(std::shared_ptr<Level> lvl, std::unique_ptr<TranslatedExp> body);

   private:
    irt::RelationOperation translateCondOp(ast::Operation op);
    irt::BinaryOperation translateArOp(ast::Operation op);
};

struct Level {
    std::shared_ptr<frame::Frame> _frame;
    std::shared_ptr<Level> _parent;
    Level(std::shared_ptr<Level> level, temp::Label f, std::vector<bool> list);
    static std::shared_ptr<Access> alloc_local(std::shared_ptr<Level> lvl, bool escape);
    static std::shared_ptr<AccessList> formals(std::shared_ptr<Level> lvl);
};

struct Access {
    std::shared_ptr<frame::Access> _access;
    std::shared_ptr<Level> _level;
    Access(std::shared_ptr<Level> level, std::shared_ptr<frame::Access> access) : _level(level), _access(access) {}
};
};    // namespace trans

#endif
