#include "translation.h"

using namespace trans;
using namespace irt;
using namespace std;
/**
 * Main translating class
 * **/

unique_ptr<TranslatedExp> Translator::simpleVar(shared_ptr<trans::Access> a,shared_ptr<trans::Level> l) {
    unique_ptr<Temp> fp = make_unique<Temp>(frame::Frame::fp);
    if (a->_level.get() == l.get()){
        return make_unique<Ex>(frame::exp(a->_access,move(fp)));
    }
    unique_ptr<Expression> staticLinkExp = frame::static_link_exp_base(move(fp));
    while (a->_level.get() != l.get()) {
        staticLinkExp = frame::static_link_jump(move(staticLinkExp));
        l = l->_parent;
    }
    return make_unique<Ex>(frame::exp_with_static_link(a->_access,move(staticLinkExp)));
}
unique_ptr<TranslatedExp> Translator::fieldVar(unique_ptr<TranslatedExp> var, int fieldIndex) {
    return make_unique<Ex>(
        make_unique<Mem>(
            make_unique<BinOp>(
                Plus,
                var->unEx(),
                make_unique<BinOp>(
                    Mul, 
                    make_unique<Const>(fieldIndex), 
                    make_unique<Const>(frame::Frame::wordSize)
                )
            )
        )
    );
}
unique_ptr<TranslatedExp> Translator::subscriptVar(unique_ptr<TranslatedExp> var,unique_ptr<TranslatedExp> sub) {
    return make_unique<Ex>(
        make_unique<Mem>(
            make_unique<BinOp>(
                Plus,
                var->unEx(),
                make_unique<BinOp>(
                    Mul, 
                    sub -> unEx(),
                    make_unique<Const>(frame::Frame::wordSize)
                )
            )
        )
    );
}

/**
 * Expression translation
 * **/
unique_ptr<TranslatedExp> Translator::unitExp() {
    return make_unique<Nx>(make_unique<Exp>(make_unique<Const>(0)));
}
unique_ptr<TranslatedExp> Translator::nilExp() {
    return make_unique<Ex>(make_unique<Const>(0));
}
unique_ptr<TranslatedExp> Translator::intExp(ast::IntExp* val) {
    return make_unique<Ex>(make_unique<Const>(val -> value));
}
unique_ptr<TranslatedExp> Translator::stringExp(ast::StringExp* str) {
  temp::Label strpos = temp::Label();
  frame::StringFrag* frag = new frame::StringFrag(strpos, str -> value);
  _frag_list -> push_back(frag);
  return make_unique<Ex>(make_unique<Name>(strpos));
}
unique_ptr<TranslatedExp> Translator::callExp(bool isLibFunc,std::shared_ptr<trans::Level> funlvl,std::shared_ptr<trans::Level> currentlvl,temp::Label name,unique_ptr<ExpressionList> list) {
    unique_ptr<irt::ExpressionList> seq = make_unique<irt::ExpressionList>();
    for (auto exp = list -> rbegin(); exp != list -> rend(); exp++){
        seq -> push_back((*exp) -> unEx());
    }
    if(!isLibFunc){
        unique_ptr<Expression> staticLink = frame::static_link_exp_base(make_unique<irt::Temp>(frame::Frame::fp));
        if (funlvl -> _parent != currentlvl){
            while(currentlvl){
                staticLink = make_unique<Mem>(move(staticLink));
                if(currentlvl -> _parent == funlvl -> _parent )
                    break;
                currentlvl = currentlvl -> _parent;
            }
        }
        seq -> push_front(move(staticLink));
    }
    return make_unique<Ex>(make_unique<Call>(make_unique<Name>(name),move(seq))); 
}
void translateArOp(ast::Operation op,BinaryOperation *tr_op){
    switch (op){
        case ast::Plus:
            *tr_op =  irt::Plus;
            break;
        case ast::Minus:
            *tr_op = irt::Minus;
            break;
        case ast::Times:
            *tr_op = irt::Mul;
            break;
        case ast::Divide:
            *tr_op = irt::Div;
            break; 
    }
}
void translateCondOp(ast::Operation op,RelationOperation *tr_op){
    switch(op){
        case ast::Eq:
            *tr_op = irt::Eq;
            break;
        case ast::Neq:
            *tr_op = irt::Ne;
            break;
        case ast::Lt:
            *tr_op = irt::Lt;
            break;
        case ast::Le:
            *tr_op = irt::Le;
            break;
        case ast::Gt:
            *tr_op = irt::Gt;
            break;
        case ast::Ge:
            *tr_op = irt::Ge;
            break;
    }
}
unique_ptr<TranslatedExp> Translator::arExp(ast::Operation op, unique_ptr<TranslatedExp> exp1,unique_ptr<TranslatedExp> exp2) {
    BinaryOperation tr_op;
    return make_unique<Ex>(make_unique<BinOp>(tr_op,exp1 -> unEx(),exp2 -> unEx()));
}
unique_ptr<TranslatedExp> Translator::condExp(ast::Operation op, unique_ptr<TranslatedExp> exp1,unique_ptr<TranslatedExp> exp2) {
    RelationOperation tr_op;
    translateCondOp(op,&tr_op);
    unique_ptr<irt::Cjump> cjump = make_unique<Cjump>(tr_op,exp1 -> unEx(),exp2 -> unEx(),nullptr,nullptr);
    PatchList trues  = PatchList(); 
    trues.push_back(&cjump->true_label);
    PatchList falses = PatchList();
    falses.push_back(&cjump->false_label);
    return make_unique<Cx>(trues, falses, move(cjump));
}
unique_ptr<TranslatedExp> Translator::strExp(ast::Operation op, unique_ptr<TranslatedExp> exp1,unique_ptr<TranslatedExp> exp2) {
    RelationOperation tr_op;
    translateCondOp(op,&tr_op);
    unique_ptr<irt::Cjump> cjump = nullptr;
    if (tr_op == irt::Eq || tr_op == irt::Ne) {
        unique_ptr<irt::ExpressionList> list = make_unique<irt::ExpressionList>();
        list -> push_back(exp1 -> unEx());
        list -> push_back(exp2 -> unEx());
        unique_ptr<irt::Expression> e = frame::external_call("stringEqual",move(list));
        cjump = make_unique<Cjump>(tr_op,move(e),make_unique<Const>(1),nullptr,nullptr);
    } else {
        unique_ptr<irt::ExpressionList> list = make_unique<irt::ExpressionList>();
        list -> push_back(exp1 -> unEx());
        list -> push_back(exp2 -> unEx());
        unique_ptr<irt::Expression> e = frame::external_call("stringCompare",move(list));
        cjump = make_unique<Cjump>(tr_op,move(e),make_unique<Const>(0),nullptr,nullptr);
    }
    PatchList trues  = PatchList(); 
    trues.push_back(&cjump->true_label);
    PatchList falses = PatchList();
    falses.push_back(&cjump->false_label);
    return make_unique<Cx>(trues, falses, move(cjump));
}
unique_ptr<TranslatedExp> Translator::recordExp(unique_ptr<trans::ExpressionList> el, int fieldCount) {
    /* Allocation */
    temp::Temp r = temp::Temp();
    unique_ptr<Move> alloc = make_unique<Move>(
            make_unique<Temp>(r),
            frame::external_call(
                "allocRecord",
                make_unique<irt::ExpressionList>(
                    new Const(fieldCount * frame::Frame::wordSize)
                    )));
    alloc -> print();
    auto exp = el -> rbegin();
    unique_ptr<Statement> result = 
        make_unique<Move>(
                make_unique<Mem>(
                    make_unique<BinOp>(
                        Plus,
                        make_unique<Temp>(r),
                        make_unique<Const>((fieldCount - 1)  * frame::Frame::wordSize)
                    )
                ),
                (*exp) -> unEx());
    exp++;
    for (; exp != el -> rend(); exp++)
    {
        result = make_unique<Seq>(
            make_unique<Move>(
                make_unique<Mem>(
                    make_unique<BinOp>(
                        Plus,
                        make_unique<Temp>(r),
                        make_unique<Const>((fieldCount - 2)  * frame::Frame::wordSize)
                    )
                ),
                (*exp) -> unEx()),
            move(result)
        );
        fieldCount--;
    }

    return make_unique<trans::Ex>(
        make_unique<Eseq>(
            make_unique<Seq>(move(alloc), move(result)),
            make_unique<Temp>(r)));
}
unique_ptr<TranslatedExp> Translator::seqExp(unique_ptr<trans::ExpressionList> list) {
    unique_ptr<Expression> res = make_unique<Const>(0);
    for (auto exp = list -> rbegin(); exp != list -> rend(); exp++)
    {
        res = make_unique<Eseq>(make_unique<Exp>(move(res)),(*exp) -> unEx());
    }
    return make_unique<Ex>(move(res));
}
unique_ptr<TranslatedExp> Translator::assignExp(unique_ptr<TranslatedExp> var,unique_ptr<TranslatedExp> exp) {
    auto a = make_unique<Nx>(make_unique<Move>(exp -> unEx(),var -> unEx()));
    a -> print();
    return move(a);
}
unique_ptr<TranslatedExp> Translator::ifExp(unique_ptr<TranslatedExp> test, unique_ptr<TranslatedExp> then, unique_ptr<TranslatedExp> elsee, seman::ExpType* if_type) {
    temp::Label* t = new temp::Label();
    temp::Label* f = new temp::Label();
    temp::Label m = temp::Label();
    Cx* res;
    if (dynamic_cast<trans::Ex*>(test.get())) {
        unique_ptr<Cx> testcx = test -> unCx();
        PatchList trues  = testcx->trues;
        PatchList falses = testcx->falses;
        res =  new Cx(trues, falses, move(testcx -> stm));
    } else if (auto rescx = dynamic_cast<trans::Cx*>(test.get()))
        res = rescx;
    else if (dynamic_cast<trans::Nx*>(test.get()))
        exit(-1);
    res -> trues.applyPatch(t);
    res -> falses.applyPatch(f);
    res -> print();
    if(if_type -> kind != seman::ExpTypeKind::NoKind  ){
        temp::Temp r = temp::Temp();
        unique_ptr<Statement> s = make_unique<Seq>(move(res->unCx() -> stm),
            make_unique<Seq>(make_unique<Label>(*t),
                make_unique<Seq>(make_unique<Move>(make_unique<Temp>(r),then -> unEx()),
                    make_unique<Seq>(make_unique<Jump>(make_unique<Name>(m),temp::LabelList(1,m)),
                        make_unique<Seq>(make_unique<Label>(*f),
                            make_unique<Seq>(make_unique<Move>(make_unique<Temp>(r),elsee -> unEx()),
                                make_unique<Label>(m)
        ))))));
        if(dynamic_cast<trans::Nx*>(then.get()))
            exit(-1);
        if(dynamic_cast<trans::Nx*>(elsee.get()))
            exit(-1);
        s -> print();
        return make_unique<Ex>(make_unique<Eseq>(move(s),make_unique<Temp>(r)));
    }
    auto a = make_unique<Nx>(
        make_unique<Seq>(move(res->unCx() -> stm),
            make_unique<Seq>(make_unique<Label>(*t),
                make_unique<Seq>(then -> unNx(),
                    make_unique<Seq>(make_unique<Jump>(make_unique<Name>(m),temp::LabelList(1,m)),
                        make_unique<Seq>(make_unique<Label>(*f),
                            make_unique<Seq>(elsee -> unNx(),
                                make_unique<Label>(m)))))))); 
    a -> print();
    return move(a);
}
unique_ptr<TranslatedExp> Translator::whileExp(unique_ptr<TranslatedExp> exp,unique_ptr<TranslatedExp> body, temp::Label breaklbl) {
    temp::Label test = temp::Label();
    temp::Label done = breaklbl;
    temp::Label loopstart = temp::Label();
    unique_ptr<Statement> s = make_unique<Seq>(make_unique<Label>(test),
        make_unique<Seq>(make_unique<Cjump>(Ne,exp -> unEx(),make_unique<Const>(0),&loopstart,&done),
            make_unique<Seq>(make_unique<Label>(loopstart),
                make_unique<Seq>(body -> unNx(),
                    make_unique<Seq>(make_unique<Jump>(make_unique<Name>(test),temp::LabelList(1,test)),
                        make_unique<Label>(done))))));
    return make_unique<Nx>(move(s));
}
unique_ptr<TranslatedExp> Translator::forExp(std::shared_ptr<trans::Access> access,std::shared_ptr<trans::Level> lvl, unique_ptr<TranslatedExp> explo, unique_ptr<TranslatedExp> exphi, unique_ptr<TranslatedExp> body, temp::Label breaklbl) {
    temp::Label test = temp::Label();
    temp::Label loopstart = temp::Label();
    temp::Temp limit = temp::Temp();
    temp::Label done = breaklbl;
    unique_ptr<Expression> var_s = simpleVar(access,lvl) -> unEx();
    unique_ptr<Expression> var_s1 = simpleVar(access,lvl) -> unEx();
    unique_ptr<Expression> var_s2 = simpleVar(access,lvl) -> unEx();
    unique_ptr<Expression> var_s3 = simpleVar(access,lvl) -> unEx();
    unique_ptr<Statement> s = make_unique<Seq>(make_unique<Move>(move(var_s),explo -> unEx()),
        make_unique<Seq>(make_unique<Label>(test),
            make_unique<Seq>(make_unique<Move>(make_unique<Temp>(limit),exphi -> unEx()),
                make_unique<Seq>(make_unique<Cjump>(Le,move(var_s3),make_unique<Temp>(limit),&loopstart,&done),
                    make_unique<Seq>(make_unique<Label>(loopstart),
                        make_unique<Seq>(body -> unNx(),
                            make_unique<Seq>(make_unique<Move>(move(var_s1),make_unique<BinOp>(Plus,move(var_s2),make_unique<Const>(1))),
                                make_unique<Seq>(make_unique<Jump>(make_unique<Name>(test),temp::LabelList(1,test)),
                                    make_unique<Label>(done)
                        ))))))));
    return make_unique<Nx>(move(s));
}
unique_ptr<TranslatedExp> Translator::letExp(unique_ptr<ExpressionList> list, unique_ptr<TranslatedExp> body) {
    unique_ptr<irt::Expression> res = body -> unEx();
    for (auto exp = list -> begin(); exp != list -> end(); exp++){
        res = make_unique<Eseq>((*exp) -> unNx(),move(res));
    }
    res -> print();
    return make_unique<Ex>(move(res));
}
unique_ptr<TranslatedExp> Translator::breakExp(temp::Label breaklbl) {
    return make_unique<Nx>(
        make_unique<Jump>(
           make_unique<Name>(breaklbl),
           temp::LabelList(1,breaklbl)
        )
    );
}
unique_ptr<TranslatedExp> Translator::arrayExp(unique_ptr<TranslatedExp> init,unique_ptr<TranslatedExp> size) {
    unique_ptr<irt::ExpressionList> list = make_unique<irt::ExpressionList>();
    list -> push_back(size -> unEx());
    list -> push_back(init -> unEx());
    list -> print();
    return make_unique<Ex>(
        frame::external_call(
            "initArray",
            move(list)
        )
    );
}

/**
 * Declaration translation
 * **/
unique_ptr<TranslatedExp> Translator::varDec() {
    // TODO: Complete
    return nullptr;
}
unique_ptr<TranslatedExp> Translator::typeDec() {
    // TODO: Complete
    return nullptr;
}
unique_ptr<TranslatedExp> Translator::funDec() {
    // TODO: Complete
    return nullptr;
}

shared_ptr<Access> Level::alloc_local(shared_ptr<Level> lvl,bool escape){
    shared_ptr<frame::Access> access = lvl ->_frame->alloc_local(escape);
    return  make_shared<Access>(lvl,access);
}

shared_ptr<AccessList> Level::formals(shared_ptr<Level> lvl) {
  shared_ptr<AccessList> trans_list = make_shared<AccessList>();
  for (auto& access : lvl -> _frame->formals()) {
    shared_ptr<Access> trans_access = make_shared<Access>(lvl,access);
    trans_list->push_back(trans_access);
  }
  return trans_list;
}
unique_ptr<TranslatedExp> Translator::nullNx(){
    return make_unique<Nx>(make_unique<Exp>(make_unique<Const>(0)));
}
unique_ptr<TranslatedExp> Translator::NoExp(){
    return make_unique<Ex>(make_unique<Const>(0));
}
