#include "IRT.h"

using namespace irt;


/**
 * Statements
 * **/
void Seq::print()   const {};
void Label::print() const {};
void Jump::print()  const {};
void Cjump::print() const {};
void Move::print()  const {};
void Exp::print()   const {};

/**
 * Expressions
 * **/
void BinOp::print() const {};
void Mem::print()   const {};
void Temp::print()  const {};
void Eseq::print()  const {};
void Name::print()  const {};
void Const::print() const {};
void Call::print()  const {};
