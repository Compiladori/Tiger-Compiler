#include "IRT.h"

using namespace irt;
using namespace std;

/**
 * Statements
 * **/
void Seq::print()   const { cout << "Seq ("; left -> print();cout<<" ) ( "; right ->print();cout<<" )"; };
void Label::print() const { cout << "Label ( ";label.print();cout<<" )"; };
void Jump::print()  const { cout << "Jump ("; exp -> print();cout<<" ) ( "; temp::print_labellist(label_list);cout<<" )"; };
void Cjump::print() const { cout << "CJump ("<< rel_op <<" ) ( "; left -> print();cout<<" ) ( "; right -> print();cout<<" ) ( "; true_label.print() ;cout<<" ) ( "; false_label.print() ;cout<<" )";};
void Move::print()  const { cout << "Move ( "; left -> print();cout<<" ) ( "; right ->print();cout<<" )";};
void Exp::print()   const {cout << "Exp ( ";exp -> print();cout<<" )"; };

/**
 * Expressions
 * **/
void BinOp::print() const {cout << "BinOp ( "<< bin_op <<" ) ( "; left -> print();cout<<" ) ( "; right -> print();cout<<" )";};
void Mem::print()   const {cout << "Mem ( ";exp->print();cout<<" )";};
void Temp::print()  const {cout << "Temp (";temporary.print();cout<<" )";};
void Eseq::print()  const {cout << "Eseq ( "; stm -> print();cout<<" ) ( "; exp ->print();cout<<" )";};
void Name::print()  const {cout << "Name ( ";name.print();cout<<" )"; };
void Const::print() const {cout << "Const ( "<< i <<" )";};
void Call::print()  const {cout << "Call ("; fun -> print();cout<<" ) ( "; args ->print();cout<<" )";};

