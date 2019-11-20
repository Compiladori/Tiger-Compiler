#ifndef __TRANSLATION_H__
#define __TRANSLATION_H__

/**
 * Translation module from Abstract Syntax into IRT
 *
 * Described in Chapter 7 Appel C (2004)
 * **/

#include "../Translation/translatedExpression.h"
#include "../Frame/frame.h"
#include "../Frame/temp.h"
#include "../Utility/utility.h"
namespace trans {
class Access;
class Level;
using AccessList = util::GenericList<Access>;

/**
 * Main translating class
 * **/
class Translator {
    // Stuff
public:
    /**
     * Variable translation
     * **/
    std::unique_ptr<TranslatedExp> simpleVar();
    std::unique_ptr<TranslatedExp> fieldVar();
    std::unique_ptr<TranslatedExp> subscriptVar();

    /**
     * Expression translation
     * **/
    std::unique_ptr<TranslatedExp> varExp();
    std::unique_ptr<TranslatedExp> unitExp();
    std::unique_ptr<TranslatedExp> nilExp();
    std::unique_ptr<TranslatedExp> intExp();
    std::unique_ptr<TranslatedExp> stringExp();
    std::unique_ptr<TranslatedExp> callExp();
    std::unique_ptr<TranslatedExp> opExp();
    std::unique_ptr<TranslatedExp> recordExp();
    std::unique_ptr<TranslatedExp> seqExp();
    std::unique_ptr<TranslatedExp> assignExp();
    std::unique_ptr<TranslatedExp> ifExp();
    std::unique_ptr<TranslatedExp> whileExp();
    std::unique_ptr<TranslatedExp> forExp();
    std::unique_ptr<TranslatedExp> letExp();
    std::unique_ptr<TranslatedExp> breakExp();
    std::unique_ptr<TranslatedExp> arrayExp();

    /**
     * Declaration translation
     * **/
    std::unique_ptr<TranslatedExp> varDec();
    std::unique_ptr<TranslatedExp> typeDec();
    std::unique_ptr<TranslatedExp> funDec();
};


struct Access {
  std::unique_ptr<frame::Access> _access;
  std::unique_ptr<Level> _level;
  Access(std::unique_ptr<Level> level,std::unique_ptr<frame::Access> access) : _level(std::move(level)), _access(std::move(access)) {}
};

struct Level {
  std::unique_ptr<frame::Frame> _frame;
  std::unique_ptr<Level> _parent;
  Level(std::unique_ptr<Level> level,temp::Label f, std::vector<bool> list);
};

std::unique_ptr<Access> alloc_local(std::unique_ptr<Level> level,bool escape);
std::unique_ptr<AccessList> formals();
std::unique_ptr<Level> outermost();
};

#endif
