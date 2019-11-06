#ifndef __IRT_H__
#define __IRT_H__

#include <string>
#include <memory>
#include "../Utility/utility.h"

/***
 * Intermediate Representation Tree (IRT)
 *
 * Described in Chapter 7 Appel C (2004)
 *
 * Intermediate language to serve as a "bridge" between source languages and compiled languages
 * ***/

/*
 * TEMPORARY ONLY TO MAKE IT COMPILE UNTIL THE TEMP MODULE IS READY
 * */
using TempLabel = int;
using LabelList = int;
using Temporary = int;

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
enum BinaryOperation {Plus, Minus, Mul, Div, And, Or, Lshift, Rshift, Arshift, Xor};
enum RelationOperation {Eq, Ne, Lt, Gt, Le, Ge, Ult, Ule, Ugt, Uge};


/**
 * Statements
 * 
 * A statement performs side effects and flow control
 * **/
struct Statement {
    virtual ~Statement() {}
    virtual void print() const = 0;
};

struct Seq   : public Statement {
	std::unique_ptr<Statement> left, right;
	
	Seq(std::unique_ptr<Statement> left, std::unique_ptr<Statement> right) : left(std::move(left)), right(std::move(right)) {}
	
    void print() const;
};

struct Label : public Statement {
	TempLabel label;
	
	Label(TempLabel label) : label(label) {}
	
    void print() const;
};

struct Jump  : public Statement {
	std::unique_ptr<Expression> exp;
	LabelList label_list;
	
	Jump(std::unique_ptr<Expression> exp, LabelList label_list) : exp(std::move(exp)), label_list(label_list) {}
	
    void print() const;
};

struct Cjump : public Statement {
	RelationOperation rel_op;
	std::unique_ptr<Expression> left, right;
	TempLabel true_label, false_label;
	
	Cjump(RelationOperation rel_op, std::unique_ptr<Expression> left, std::unique_ptr<Expression> right, TempLabel true_label, TempLabel false_label) :
		rel_op(rel_op), left(std::move(left)), right(std::move(right)), true_label(true_label), false_label(false_label) {}
	
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
    virtual ~Expression() {}
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
    Temporary temp;
    
    Temp(Temporary temp) : temp(temp) {}
    
    void print() const;
};

struct Eseq  : public Expression {
	std::unique_ptr<Statement> stm;
	std::unique_ptr<Expression> exp;
	
	Eseq(std::unique_ptr<Statement> stm, std::unique_ptr<Expression> exp) : stm(std::move(stm)), exp(std::move(exp)) {}
	
    void print() const;
};

struct Name  : public Expression {
    TempLabel name;
    
    Name(TempLabel name) : name(name) {}
    
    void print() const;
};

struct Const : public Expression {
    int i;
    
    Const(int i) : i(i) {}
    
    void print() const;
};

struct Call  : public Expression {
    void print() const;
};




};

#endif
