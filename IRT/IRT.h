#ifndef __IRT_H__
#define __IRT_H__

#include <string>
#include <memory>
#include <vector>
#include "../Utility/utility.h"
#include "../Frame/temp.h"

/***
 * Intermediate Representation Tree (IRT)
 *
 * Described in Chapter 7 Appel C (2004)
 *
 * Intermediate language to serve as a "bridge" between source languages and compiled languages
 * ***/


namespace irt {

/**
 * Forward declarations
 * **/
class Expression;
class Statement;

using ExpressionList = util::GenericList<Expression>;
using StatementList  = util::GenericList<Statement>;


/**
 * Operations
 * **/
enum BinaryOperation {Plus, Minus, Mul, Div};
enum RelationOperation {Eq, Ne, Lt, Gt, Le, Ge};


/**
 * Statements
 *
 * A statement performs side effects and flow control
 * **/
struct Statement {
    virtual ~Statement() = default;
    virtual void print() const = 0;
};

struct Seq   : public Statement {
	std::unique_ptr<Statement> left, right;

	Seq(std::unique_ptr<Statement> left, std::unique_ptr<Statement> right) : left(std::move(left)), right(std::move(right)) {}

    void print() const;
};

struct Label : public Statement {
	temp::Label label;

	Label(temp::Label label) : label(label) {}

    void print() const;
};

struct Jump  : public Statement {
	std::unique_ptr<Expression> exp;
	temp::LabelList label_list;

	Jump(std::unique_ptr<Expression> exp, temp::LabelList label_list) : exp(std::move(exp)), label_list(label_list) {}

    void print() const;
};

struct Cjump : public Statement {
	RelationOperation rel_op;
	std::unique_ptr<Expression> left, right;
	temp::Label true_label;
    temp::Label false_label;

	Cjump(RelationOperation rel_op, std::unique_ptr<Expression> left, std::unique_ptr<Expression> right, temp::Label _true_label, temp::Label _false_label) :
		rel_op(rel_op), left(std::move(left)), right(std::move(right)), true_label(_true_label), false_label(_false_label) {}
    Cjump(RelationOperation rel_op, std::unique_ptr<Expression> left, std::unique_ptr<Expression> right) :
		rel_op(rel_op), left(std::move(left)), right(std::move(right)), true_label(temp::Label("noInit")), false_label(temp::Label("noInit")) {}
    void print() const;
};

struct Move  : public Statement {
	std::unique_ptr<Expression> left, right;

	Move(std::unique_ptr<Expression> left, std::unique_ptr<Expression> right) : left(std::move(left)), right(std::move(right)) {}

    void print() const;
};

struct Exp   : public Statement {
	std::unique_ptr<Expression> exp;

	Exp(std::unique_ptr<Expression> exp) : exp(std::move(exp)) {}

    void print() const;
};


/**
 * Expressions
 *
 * An expression represents a computation of some value (possibly with side effects)
 * **/
struct Expression {
    virtual ~Expression() = default;
    virtual void print() const = 0;
};


struct BinOp : public Expression {
	BinaryOperation bin_op;
	std::unique_ptr<Expression> left, right;

	BinOp(BinaryOperation bin_op, std::unique_ptr<Expression> left, std::unique_ptr<Expression> right) :
		bin_op(bin_op), left(std::move(left)), right(std::move(right)) {}

    void print() const;
};

struct Mem   : public Expression {
	std::unique_ptr<Expression> exp;

	Mem(std::unique_ptr<Expression> exp) : exp(std::move(exp)) {}

    void print() const;
};

struct Temp  : public Expression {
    temp::Temp temporary;

    Temp(temp::Temp temporary) : temporary(temporary) {}

    void print() const;
};

struct Eseq  : public Expression {
	std::unique_ptr<Statement> stm;
	std::unique_ptr<Expression> exp;

	Eseq(std::unique_ptr<Statement> stm, std::unique_ptr<Expression> exp) : stm(std::move(stm)), exp(std::move(exp)) {}

    void print() const;
};

struct Name  : public Expression {
    temp::Label name;

    Name(temp::Label name) : name(name) {}

    void print() const;
};

struct Const : public Expression {
    int i;

    Const(int i) : i(i) {}

    void print() const;
};

struct Call  : public Expression {
    std::unique_ptr<Expression> fun;
    std::unique_ptr<ExpressionList> args;

    Call(std::unique_ptr<Expression> fun, std::unique_ptr<ExpressionList> args) :
        fun(std::move(fun)), args(std::move(args)) {}

    void print() const;
};




};

#endif
