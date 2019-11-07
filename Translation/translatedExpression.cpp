#include "translatedExpression.h"

using namespace trans;

/**
 * Ex (expression)
 * **/
std::unique_ptr<irt::Expression> Ex::unEx() const {
    // TODO: Complete
    return nullptr;
}
std::unique_ptr<irt::Statement>  Ex::unNx() const {
    // TODO: Complete
    return nullptr;
}
std::unique_ptr<Cx>              Ex::unCx() const {
    // TODO: Complete
    return nullptr;
}
void Ex::print() const {}

/**
 * Nx (no result)
 * **/
std::unique_ptr<irt::Expression> Nx::unEx() const {
    // TODO: Complete
    return nullptr;
}
std::unique_ptr<irt::Statement>  Nx::unNx() const {
    // TODO: Complete
    return nullptr;
}
std::unique_ptr<Cx>              Nx::unCx() const {
    // TODO: Complete
    return nullptr;
}
void Nx::print() const {}

/**
 * Cx (conditional)
 * **/
std::unique_ptr<irt::Expression> Cx::unEx() const {
    // TODO: Complete
    return nullptr;
}
std::unique_ptr<irt::Statement>  Cx::unNx() const {
    // TODO: Complete
    return nullptr;
}
std::unique_ptr<Cx>              Cx::unCx() const {
    // TODO: Complete
    return nullptr;
}
void Cx::print() const {}
