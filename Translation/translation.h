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
    std::unique_ptr<frame::FragList> _frag_list;
public:
    /**
     * Variable translation
     * **/
    std::unique_ptr<TranslatedExp> simpleVar(Access* a,Level* l);
    std::unique_ptr<TranslatedExp> fieldVar(std::unique_ptr<TranslatedExp> var, int fieldIndex);
    std::unique_ptr<TranslatedExp> subscriptVar(std::unique_ptr<TranslatedExp> var,std::unique_ptr<TranslatedExp> sub);

    /**
     * Expression translation
     * **/
    std::unique_ptr<TranslatedExp> unitExp();
    std::unique_ptr<TranslatedExp> nilExp();
    std::unique_ptr<TranslatedExp> intExp(std::unique_ptr<ast::IntExp> val);
    std::unique_ptr<TranslatedExp> stringExp(std::unique_ptr<ast::StringExp> str);
    std::unique_ptr<TranslatedExp> callExp();
    std::unique_ptr<TranslatedExp> arExp(ast::Operation op, std::unique_ptr<TranslatedExp> exp1,std::unique_ptr<TranslatedExp> exp2);
    std::unique_ptr<TranslatedExp> condExp(ast::Operation op, std::unique_ptr<TranslatedExp> exp1,std::unique_ptr<TranslatedExp> exp2);
    std::unique_ptr<TranslatedExp> strExp(ast::Operation op, std::unique_ptr<TranslatedExp> exp1,std::unique_ptr<TranslatedExp> exp2);
    std::unique_ptr<TranslatedExp> recordExp(std::unique_ptr<trans::ExpressionList> el, int fieldCount);
    std::unique_ptr<TranslatedExp> seqExp(std::unique_ptr<trans::ExpressionList> list);
    std::unique_ptr<TranslatedExp> assignExp(std::unique_ptr<TranslatedExp> var,std::unique_ptr<TranslatedExp> exp);
    std::unique_ptr<TranslatedExp> ifExp();
    std::unique_ptr<TranslatedExp> whileExp(std::unique_ptr<TranslatedExp> exp,std::unique_ptr<TranslatedExp> body, temp::Label breaklbl);
    std::unique_ptr<TranslatedExp> forExp(trans::Access* access,trans::Level* lvl, std::unique_ptr<TranslatedExp> explo, std::unique_ptr<TranslatedExp> exphi, std::unique_ptr<TranslatedExp> body, temp::Label breaklbl);
    std::unique_ptr<TranslatedExp> letExp();
    std::unique_ptr<TranslatedExp> breakExp(temp::Label breaklbl);
    std::unique_ptr<TranslatedExp> arrayExp(std::unique_ptr<TranslatedExp> init,std::unique_ptr<TranslatedExp> size);

    /**
     * Declaration translation
     * **/
    std::unique_ptr<TranslatedExp> varDec();
    std::unique_ptr<TranslatedExp> typeDec();
    std::unique_ptr<TranslatedExp> funDec();
};

struct Level {
  frame::Frame* _frame;
  Level* _parent;
  Level(Level* level,temp::Label f, std::vector<bool> list) : _parent(level), _frame(new frame::Frame(f,list)) {}
};


struct Access {
  frame::Access* _access;
  Level* _level;
  Access(Level *level,frame::Access *access) : _level(level), _access(access) {}
};

std::unique_ptr<Access> alloc_local(std::unique_ptr<Level> level,bool escape);
std::unique_ptr<AccessList> formals(std::unique_ptr<Level> l);
Level outermost();
};

#endif
