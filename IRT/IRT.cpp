#include "IRT.h"

using namespace irt;
using namespace std;

/**
 * Statements
 * **/
void Seq::print()   const { cout << "Seq ("; left -> print();" ) ( "; right ->print();" )"; };
void Label::print() const { cout << "Label ";label.print(); };
void Jump::print()  const { cout << "Jump ("; exp -> print();" ) ( "; temp::print_labellist(label_list);" )"; };
void Cjump::print() const { cout << "CJump ("<< rel_op <<" ) ( "; left -> print();" ) ( "; right -> print();" ) ( "; true_label -> print() ;" ) ( "; false_label -> print() ;" )";};
void Move::print()  const { cout << "Move ("; left -> print();" ) ( "; right ->print();" )";};
void Exp::print()   const {cout << "Exp (";exp -> print();" )"; };

/**
 * Expressions
 * **/
void BinOp::print() const {cout << "BinOp ("<< bin_op <<" ) ( "; left -> print();" ) ( "; right -> print();" )";};
void Mem::print()   const {cout << "Mem (";exp->print();" )";};
void Temp::print()  const {cout << "Temp ";temporary.print();};
void Eseq::print()  const {cout << "Eseq ("; stm -> print();" ) ( "; exp ->print();" )";};
void Name::print()  const {cout << "Name ";name.print();};
void Const::print() const {cout << "Const ("<< i <<" )";};
void Call::print()  const {cout << "Call ("; fun -> print();" ) ( "; args ->print();" )";};

