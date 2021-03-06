#include "translatedExpression.h"

using namespace trans;
using namespace std;

/**
 * Implementation of the translatedExpression.h header
 * 
 * The Move(left,right) IRT statements are built using Intel Syntax:
 * move destination, source
 * */

/**
 * Ex (expression)
 * **/
unique_ptr<irt::Expression> Ex::unEx()  {
  return move(exp);
}
unique_ptr<irt::Statement>  Ex::unNx()  {
    return make_unique<irt::Exp>(move(exp));
}
unique_ptr<Cx>              Ex::unCx()  {
  unique_ptr<irt::Cjump> cjump = make_unique<irt::Cjump>(irt::Ne, this->unEx(), make_unique<irt::Const>(0));
  PatchList trues  = PatchList(); 
  trues.push_back(&cjump->true_label);
  PatchList falses = PatchList();
  falses.push_back(&cjump->false_label);
  return make_unique<Cx>(trues, falses, move(cjump));
}
void Ex::print()  {cout <<"Ex ("; exp -> print(); cout << " )";}

/**
 * Nx (no result)
 * **/
unique_ptr<irt::Expression> Nx::unEx() {
    return make_unique<irt::Eseq>(move(stm), make_unique<irt::Const>(0));
}
unique_ptr<irt::Statement>  Nx::unNx()  {
    return move(stm);
}
unique_ptr<Cx>              Nx::unCx()  {

  throw error::internal_error("Unable to unpack nx to cx", __FILE__);

}
void Nx::print()  {cout <<"Nx ("; stm -> print(); cout <<" )"; }

/**
 * Cx (conditional)
 * **/
unique_ptr<irt::Expression> Cx::unEx()  {
    temp::Temp t_temp = temp::Temp();

    temp::Label t = temp::Label(), f = temp::Label();

    unique_ptr<irt::Label> irt_label_t = make_unique<irt::Label>(t);
    unique_ptr<irt::Label> irt_label_f = make_unique<irt::Label>(f);

    unique_ptr<irt::Const> T0 = make_unique<irt::Const>(0);
    unique_ptr<irt::Const> T1 = make_unique<irt::Const>(1);
    unique_ptr<irt::Move> move1 = make_unique<irt::Move>(make_unique<irt::Temp>(t_temp), move(T1));
    unique_ptr<irt::Move> move0 = make_unique<irt::Move>(make_unique<irt::Temp>(t_temp), move(T0));
    trues.applyPatch(t);
    falses.applyPatch(f);
    return make_unique<irt::Eseq>(move(move1),
            make_unique<irt::Eseq>(move(stm),
              make_unique<irt::Eseq>(move(irt_label_f),
                make_unique<irt::Eseq>(move(move0),
                  make_unique<irt::Eseq>(move(irt_label_t),
                    make_unique<irt::Temp>(t_temp))))));
}
unique_ptr<irt::Statement>  Cx::unNx()  {
    temp::Temp t_temp = temp::Temp();
    temp::Label t = temp::Label(), f = temp::Label();

    unique_ptr<irt::Label> irt_label_t = make_unique<irt::Label>(t);
    unique_ptr<irt::Label> irt_label_f = make_unique<irt::Label>(f);

    unique_ptr<irt::Const> T0 = make_unique<irt::Const>(0);
    unique_ptr<irt::Const> T1 = make_unique<irt::Const>(1);
    unique_ptr<irt::Move> move1 = make_unique<irt::Move>(make_unique<irt::Temp>(t_temp), move(T1));
    unique_ptr<irt::Move> move0 = make_unique<irt::Move>(make_unique<irt::Temp>(t_temp), move(T0));
    trues.applyPatch(t);
    falses.applyPatch(f);
    return make_unique<irt::Seq>(move(move1),
            make_unique<irt::Seq>(move(stm),
              make_unique<irt::Seq>(move(irt_label_f),
                make_unique<irt::Seq>(move(move0),
                  make_unique<irt::Seq>(move(irt_label_t),
                    make_unique<irt::Exp>(make_unique<irt::Temp>(t_temp)))))));
}
unique_ptr<Cx>              Cx::unCx()  {
    return make_unique<Cx>( trues, falses, move(stm));
}
void Cx::print()  {cout <<"Cx ("; stm -> print();" )";}
