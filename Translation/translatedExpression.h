#ifndef __TRANSLATED_EXPRESSION_H__
#define __TRANSLATED_EXPRESSION_H__

#include "../IRT/IRT.h"

/* TEMPORARY UNTIL DEFINED */
using PatchList = int;

namespace trans {

/**
 * Modules used in the translation process
 * **/
class TranslatedExp {
public:
    virtual void print() const = 0;
};

/**
 * Ex stands for an "Expression"
 * **/
struct Ex : public TranslatedExp {
    
    Ex(std::unique_ptr<irt::Expression> exp) {}

    void print() const override {}
};

/**
 * Nx stands for "No Result"
 * **/
struct Nx : public TranslatedExp {
    
    Nx(std::unique_ptr<irt::Statement> stm) {}

    void print() const override {}
};

/**
 * Cx stands for "Conditional"
 * **/
struct Cx : public TranslatedExp {

    Cx(PatchList trues, PatchList falses, std::unique_ptr<irt::Statement> stm) {}

    void print() const override {}
};


};

#endif
