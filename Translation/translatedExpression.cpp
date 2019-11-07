#include "translatedExpression.h"

using namespace trans;

/**
 * Ex (expression)
 * **/
std::unique_ptr<irt::Expression> Ex::unEx() const {}
std::unique_ptr<irt::Statement>  Ex::unNx() const {}
std::unique_ptr<Cx>              Ex::unCx() const {}
void Ex::print() const {}

/**
 * Nx (no result)
 * **/
std::unique_ptr<irt::Expression> Nx::unEx() const {}
std::unique_ptr<irt::Statement>  Nx::unNx() const {}
std::unique_ptr<Cx>              Nx::unCx() const {}
void Nx::print() const {}

/**
 * Cx (conditional)
 * **/
std::unique_ptr<irt::Expression> Cx::unEx() const {}
std::unique_ptr<irt::Statement>  Cx::unNx() const {}
std::unique_ptr<Cx>              Cx::unCx() const {}
void Cx::print() const {}
