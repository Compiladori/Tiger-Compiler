#ifndef __TRANSLATION_H__
#define __TRANSLATION_H__

/**
 * Translation module from Abstract Syntax into IRT
 * 
 * Described in Chapter 7 Appel C (2004)
 * **/

#include "translatedExpression.h"

namespace trans {


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



};

#endif
