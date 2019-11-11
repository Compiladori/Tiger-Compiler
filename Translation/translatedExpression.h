#ifndef __TRANSLATED_EXPRESSION_H__
#define __TRANSLATED_EXPRESSION_H__

#include "../IRT/IRT.h"
#include "patchList.h"


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
    virtual std::unique_ptr<irt::Expression> unEx() const = 0;
    virtual std::unique_ptr<irt::Statement>  unNx() const = 0;
    virtual std::unique_ptr<Cx>              unCx() const = 0;

    virtual void print() const = 0;
};

/**
 * Ex stands for an "Expression"
 * **/
struct Ex : public TranslatedExp {

    Ex(std::unique_ptr<irt::Expression> exp) {}

    virtual std::unique_ptr<irt::Expression> unEx() const override;
    virtual std::unique_ptr<irt::Statement>  unNx() const override;
    virtual std::unique_ptr<Cx>              unCx() const override;
    void print() const override;
};

/**
 * Nx stands for "No Result"
 * **/
struct Nx : public TranslatedExp {

    Nx(std::unique_ptr<irt::Statement> stm) {}

    virtual std::unique_ptr<irt::Expression> unEx() const override;
    virtual std::unique_ptr<irt::Statement>  unNx() const override;
    virtual std::unique_ptr<Cx>              unCx() const override;

    void print() const override;
};

/**
 * Cx stands for "Conditional"
 * **/
struct Cx : public TranslatedExp {

    Cx(PatchList trues, PatchList falses, std::unique_ptr<irt::Statement> stm) {}

    virtual std::unique_ptr<irt::Expression> unEx() const override;
    virtual std::unique_ptr<irt::Statement>  unNx() const override;
    virtual std::unique_ptr<Cx>              unCx() const override;

    void print() const override;
};

// check
struct CanonicalTree : public TranslatedExp {
    CanonicalTree(std::unique_ptr<irt::Statement> stm) {}
    virtual std::unique_ptr<irt::Expression> unEx() const override;
    virtual std::unique_ptr<irt::Statement>  unNx() const override;
    virtual std::unique_ptr<Cx>              unCx() const override;
    irt::StatementList linearize();
};

};

#endif
