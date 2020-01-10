#include "translation.h"

using namespace trans;
using namespace irt;
using namespace std;
/**
 * Main translating class
 * **/

unique_ptr<TranslatedExp> Translator::simpleVar(Access* a,Level* l) {
    unique_ptr<Temp> fp = make_unique<Temp>(frame::Frame::fp);
    if (a->_level == l)
        return make_unique<Ex>(frame::exp(a->_access,move(fp)));
    unique_ptr<Expression> staticLinkExp = frame::static_link_exp_base(move(fp));
    while (a->_level != l) {
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
    return make_unique<Nx>(nullptr);
}
unique_ptr<TranslatedExp> Translator::nilExp() {
    return make_unique<Ex>(make_unique<Const>(0));
}
unique_ptr<TranslatedExp> Translator::intExp(unique_ptr<ast::IntExp> val) {
    return make_unique<Ex>(make_unique<Const>(val -> value));
}
unique_ptr<TranslatedExp> Translator::stringExp(unique_ptr<ast::StringExp> str) {
  temp::Label strpos = temp::Label();
  frame::StringFrag* frag = new frame::StringFrag(strpos, str -> value);
  _frag_list -> push_back(frag);
  return make_unique<Ex>(make_unique<Name>(strpos));
}
unique_ptr<TranslatedExp> Translator::callExp() {
    // TODO: Complete
    return nullptr;
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
    PatchList trues  = {cjump->true_label};
    PatchList falses = {cjump->false_label};
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
    PatchList trues  = {cjump->true_label};
    PatchList falses = {cjump->false_label};
    return make_unique<Cx>(trues, falses, move(cjump));
}
unique_ptr<TranslatedExp> Translator::recordExp(unique_ptr<trans::ExpressionList> el, int fieldCount) {
    /* Allocation */
    temp::Temp r = temp::Temp();
    unique_ptr<Statement> alloc = make_unique<Move>(
            make_unique<Temp>(r),
            frame::external_call(
                "allocRecord",
                make_unique<irt::ExpressionList>(
                    new Const(fieldCount * frame::Frame::wordSize)
                    )));
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
    for (; exp != el -> rend(); exp++)
    {
        result = make_unique<Seq>(
            make_unique<Move>(
                make_unique<Mem>(
                    make_unique<BinOp>(
                        Plus,
                        make_unique<Temp>(r),
                        make_unique<Const>((fieldCount - 1)  * frame::Frame::wordSize)
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
    return make_unique<Nx>(make_unique<Move>(var -> unEx(),exp -> unEx()));
}
unique_ptr<TranslatedExp> Translator::ifExp() {
    // TODO: Complete
    return nullptr;
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
unique_ptr<TranslatedExp> Translator::forExp(trans::Access* access,trans::Level* lvl, unique_ptr<TranslatedExp> explo, unique_ptr<TranslatedExp> exphi, unique_ptr<TranslatedExp> body, temp::Label breaklbl) {
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
unique_ptr<TranslatedExp> Translator::letExp() {
    // TODO: Complete
    return nullptr;
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
    unique_ptr<irt::ExpressionList> list = make_unique<irt::ExpressionList>((size -> unEx()).get());
    list -> push_back((init -> unEx()).get());
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

unique_ptr<trans::Access> alloc_local(unique_ptr<Level> level,bool escape){
    unique_ptr<frame::Access> access = level->_frame->alloc_local(escape);
    return make_unique<Access>(level.get(),access.get());
}

unique_ptr<AccessList> formals(unique_ptr<Level> level) {
  unique_ptr<AccessList> trans_list = make_unique<AccessList>();
  for (auto& i : level->_frame->formals()) {
    unique_ptr<Access> trans_access = make_unique<Access>(level.get(),i.get());
    trans_list->push_back(move(trans_access));
  }
  return trans_list;
}

Level* outermost(){
    temp::Label tiger_label = temp::Label("mainLevel");
    return new trans::Level(nullptr,tiger_label,vector<bool>());
}
