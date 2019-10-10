#include <iostream>
#include <type_traits>
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
void TypeField::print()     const { cout << "TypeField ("; id -> print(); cout <<") ("; type_id -> print(); cout << ")"; }
void RecordField::print()   const { cout << "RecordField ("; id -> print(); cout <<") ("; exp -> print(); cout << ")"; }

void NameType::print()      const { cout << "NameType ("; type_id -> print(); cout << ")"; }
void RecordType::print()    const { cout << "RecordType ("; tyfields -> print(); cout << ")"; }
void ArrayType::print()     const { cout << "ArrayType ("; type_id -> print(); cout << ")"; }



/** Variables **/
void SimpleVar::print()     const { cout << "SimpleVar ("; id -> print(); cout << ")"; }
void FieldVar::print()      const { cout << "FieldVar ("; var -> print(); cout << ") ("; id -> print(); cout << ")"; }
void SubscriptVar::print()  const { cout << "SubscriptVar ("; var -> print(); cout << ") ("; exp -> print(); cout << ")"; }



/** Expressions */
void VarExp::print()    const { cout << "VarExp ("; var -> print(); cout << ")"; }
void UnitExp::print()   const { cout << "UnitExp ()"; }
void NilExp::print()    const { cout << "NilExp ()"; }
void CallExp::print()   const { cout << "CallExp ("; func -> print(); cout << ") ("; exp_list -> print(); cout << ")"; }
void OpExp::print()     const { cout << "OpExp ("; left -> print(); cout << ") " << oper << " ("; right -> print(); cout << ")"; }
void RecordExp::print() const { cout << "RecordExp ("; fields -> print(); cout << ") ("; type_id -> print(); cout << ")"; }
void SeqExp::print()    const { cout << "SeqExp ("; exp_list -> print(); cout << ")"; }
void AssignExp::print() const { cout << "AssignExp ("; var -> print(); cout << ") ("; exp -> print(); cout << ")"; }
void IfExp::print()     const {
    cout << "IfExp ("; test -> print(); cout << ") ("; then -> print(); cout << ") (";
    if(otherwise) otherwise->print(); else cout << "None";
    cout << ")";
}
void WhileExp::print()  const { cout << "WhileExp ("; test -> print(); cout << ") ("; body -> print(); cout << ")"; }
void ForExp::print()    const { cout << "ForExp ("; var -> print(); cout << ") ("; lo -> print(); cout << ") ("; hi -> print(); cout << ") ("; body -> print(); cout << ")"; }
void LetExp::print()    const { cout << "LetExp (";  decs -> print(); cout << ") ("; body -> print(); cout << ")"; }
void BreakExp::print()  const { cout << "BreakExp ()"; }
void ArrayExp::print()  const { cout << "ArrayExp ("; ty -> print(); cout << ") ("; size -> print(); cout << ") ("; init -> print(); cout << ")"; }



/** Declarations **/
void VarDec::print()    const {
    cout << "VarDec ("; id -> print(); cout << ") (";
    if(type_id) type_id->print(); else cout << "None";
    cout << ") ("; exp -> print(); cout << ")";
}
void TypeDec::print()   const { cout << "TypeDec ("; type_id -> print(); cout << ") ("; ty -> print(); cout << ")"; }
void FunDec::print()    const {
    cout << "FunDec ("; id -> print(); cout << ") ("; tyfields -> print(); cout << ") (";
    if(type_id) type_id->print(); else cout << "None";
    cout << ") ("; exp -> print(); cout << ")";
}

void GroupedDeclarations::frontAppendDeclaration(Declaration *dec){
    if(this->empty() or dynamic_cast<VarDec*>(dec)) {
        // Create a new group if we had no group to join to, or we're a variable declaration
        this->push_front(new DeclarationList(dec));
    } else {
        // Join last group if their types match
        auto p = this->front()->front().get();
        
        bool matchTypeDec = dynamic_cast<TypeDec*>(p) and dynamic_cast<TypeDec*>(dec);
        bool matchFunDec = dynamic_cast<FunDec*>(p) and dynamic_cast<FunDec*>(dec);
        
        if(matchTypeDec or matchFunDec) this->front()->push_front(dec);
        else                            this->push_front(new DeclarationList(dec));
    }
}




