#include <iostream>
#include "AST.h"

using namespace ast;
using std::cout;
using std::endl;


/** Operations **/
std::string operation_name[] = { [Operation::Plus] = "+",
                                 [Operation::Minus] = "-",
                                 [Operation::Times] = "*",
                                 [Operation::Divide] = "/",
                                 [Operation::Eq] = "=",
                                 [Operation::Neq] = "!=",
                                 [Operation::Lt] = "<",
                                 [Operation::Le] = "<=" ,
                                 [Operation::Gt] = ">", 
                                 [Operation::Ge] = ">=" };
                                 
std::ostream& operator<<(std::ostream& os, const Operation& op){ return os << operation_name[op]; }



/** Types **/
void TypeField::print(){ cout << "TypeField ("; id -> print(); cout <<") ("; type_id -> print(); cout << ")"; }
void RecordField::print(){ cout << "RecordField ("; id -> print(); cout <<") ("; exp -> print(); cout << ")"; }

void NameType::print(){ cout << "NameType ("; type_id -> print(); cout << ")"; }
void RecordType::print(){ cout << "RecordType ("; tyfields -> print(); cout << ")"; }
void ArrayType::print(){ cout << "ArrayType ("; type_id -> print(); cout << ")"; }



/** Variables **/
void SimpleVar::print(){ cout << "SimpleVar ("; id -> print(); cout << ")"; }
void FieldVar::print(){ cout << "FieldVar ("; var -> print(); cout << ") ("; id -> print(); cout << ")"; }
void SubscriptVar::print(){ cout << "SubscriptVar ("; var -> print(); cout << ") ("; exp -> print(); cout << ")"; }



/** Expressions */
void VarExp::print(){ cout << "VarExp ("; var -> print(); cout << ")"; }
void UnitExp::print(){ cout << "UnitExp ()"; }
void NilExp::print(){ cout << "NilExp ()"; }
void CallExp::print(){ cout << "CallExp ("; func -> print(); cout << ") ("; exp_list -> print(); cout << ")"; }
void OpExp::print(){ cout << "OpExp ("; left -> print(); cout << ") " << oper << " ("; right -> print(); cout << ")"; }
void RecordExp::print(){ cout << "RecordExp ("; fields -> print(); cout << ") ("; type_id -> print(); cout << ")"; }
void SeqExp::print(){ cout << "SeqExp ("; exp_list -> print(); cout << ")"; }
void AssignExp::print(){ cout << "AssignExp ("; var -> print(); cout << ") ("; exp -> print(); cout << ")"; }
void IfExp::print(){
    cout << "IfExp ("; test -> print(); cout << ") ("; then -> print(); cout << ") (";
    if(otherwise) otherwise->print(); else cout << "None";
    cout << ")";
}
void WhileExp::print(){ cout << "WhileExp ("; test -> print(); cout << ") ("; body -> print(); cout << ")"; }
void ForExp::print(){ cout << "ForExp ("; var -> print(); cout << ") ("; lo -> print(); cout << ") ("; hi -> print(); cout << ") ("; body -> print(); cout << ")"; }
void LetExp::print(){ cout << "LetExp (";  decs -> print(); cout << ") ("; body -> print(); cout << ")"; }
void BreakExp::print(){ cout << "BreakExp ()"; }
void ArrayExp::print(){ cout << "ArrayExp ("; ty -> print(); cout << ") ("; size -> print(); cout << ") ("; init -> print(); cout << ")"; }



/** Declarations **/
void VarDec::print(){
    cout << "VarDec ("; id -> print(); cout << ") (";
    if(type_id) type_id->print(); else cout << "None";
    cout << ") ("; exp -> print(); cout << ")";
}
void TypeDec::print(){ cout << "TypeDec ("; type_id -> print(); cout << ") ("; ty -> print(); cout << ")"; }
void FunDec::print(){
    cout << "FunDec ("; id -> print(); cout << ") ("; tyfields -> print(); cout << ") (";
    if(type_id) type_id->print(); else cout << "None";
    cout << ") ("; exp -> print(); cout << ")";
}

void GroupedDeclarations::appendDeclaration(Declaration *dec){
    if(this->empty() or dec->getKind() == DeclarationKind::VarDK) {
        // Create a new group if we had no group to join to, or we're a variable declaration
        this->push_back(new DeclarationList(dec));
    } else {
        // Join last group if its Kind matches
        if(this->back()->back()->getKind() == dec->getKind()) this->back()->push_back(dec);
        else                                                  this->push_back(new DeclarationList(dec));
    }
}




