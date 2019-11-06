#include "IRT.h"

using namespace irt;

/**
 * Statements
 * **/
void Seq::print()   const override {};
void Label::print() const override {};
void Jump::print()  const override {};
void Cjump::print() const override {};
void Move::print()  const override {};
void Exp::print()   const override {};

/**
 * Expressions
 * **/
void BinOp::print() const override {};
void Mem::print()   const override {};
void Temp::print()  const override {};
void Eseq::print()  const override {};
void Name::print()  const override {};
void Const::print() const override {};
void Call::print()  const override {};
