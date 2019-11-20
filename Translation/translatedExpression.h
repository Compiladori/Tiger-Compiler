#ifndef __TRANSLATED_EXPRESSION_H__
#define __TRANSLATED_EXPRESSION_H__

#include "../IRT/IRT.h"
#include "patchList.h"
#include "../Utility/error.h"


namespace trans {

/**
 * Forward declarations
 * **/
class TranslatedExp;
class Ex;
class Nx;
class Cx;


/**
 * Modules used in the translation process
 * **/
class TranslatedExp {
public:
    virtual std::unique_ptr<irt::Expression> unEx()  = 0;
    virtual std::unique_ptr<irt::Statement>  unNx()  = 0;
    virtual std::unique_ptr<Cx>              unCx()  = 0;

    virtual void print()  = 0;
};

/**
 * Ex stands for an "Expression"
 * **/
struct Ex : public TranslatedExp {
    std::unique_ptr<irt::Expression> exp;

    Ex() = default; // TODO: Check if actually valid
    Ex(std::unique_ptr<irt::Expression> exp) : exp(std::move(exp)) {}

    virtual std::unique_ptr<irt::Expression> unEx()  override;
    virtual std::unique_ptr<irt::Statement>  unNx()  override;
    virtual std::unique_ptr<Cx>              unCx()  override;
    void print()  override;
};

/**
 * Nx stands for "No Result"
 * **/
struct Nx : public TranslatedExp {
    std::unique_ptr<irt::Statement> stm;

    Nx() = default; // TODO: Check if actually valid
    Nx(std::unique_ptr<irt::Statement> stm) : stm(std::move(stm)) {}

    virtual std::unique_ptr<irt::Expression> unEx()  override;
    virtual std::unique_ptr<irt::Statement>  unNx()  override;
    virtual std::unique_ptr<Cx>              unCx()  override;

    void print()  override;
};

/**
 * Cx stands for "Conditional"
 * **/
struct Cx : public TranslatedExp {
    PatchList trues, falses;
    std::unique_ptr<irt::Statement> stm;

    Cx() = default; // TODO: Check if actually valid
    Cx(PatchList trues, PatchList falses, std::unique_ptr<irt::Statement> stm) : trues(trues), falses(falses), stm(std::move(stm)) {}

    virtual std::unique_ptr<irt::Expression> unEx()  override;
    virtual std::unique_ptr<irt::Statement>  unNx()  override;
    virtual std::unique_ptr<Cx>              unCx()  override;

    void print()  override;
};


};

#endif
