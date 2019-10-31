#ifndef __IRT_H__
#define __IRT_H__

/***
 * Intermediate Representation Tree (IRT)
 *
 * Described in Chapter 7 Appel C (2004)
 *
 * Intermediate language to serve as a "bridge" between source languages and compiled languages
 * ***/

namespace irt {

enum BinaryOperation {Plus, Minus, Mul, Div, And, Or, Lshift, Rshift, Arshift, Xor};
enum RelationOperation {Eq, Ne, Lt, Gt, Le, Ge, Ult, Ule, Ugt, Uge};


/**
 * Statements
 * 
 * A statement performs side effects and flow control
 * **/
struct Statement {
    virtual ~Statement() {}
    virtual void print() = 0;
};
struct Seq   : public Statement { void print() const; };
struct Label : public Statement { void print() const; };
struct Jump  : public Statement { void print() const; };
struct Cjump : public Statement { void print() const; };
struct Move  : public Statement { void print() const; };
struct Exp   : public Statement { void print() const; };


/**
 * Expressions
 * 
 * An expression represents a computation of some value (possibly with side effects)
 * **/
struct Expression {
    virtual ~Expression() {}
    virtual void print() const = 0;
};
struct BinOp : public Expression { void print() const; };
struct Mem   : public Expression { void print() const; };
struct Temp  : public Expression { void print() const; };
struct Eseq  : public Expression { void print() const; };
struct Name  : public Expression { void print() const; };
struct Const : public Expression { void print() const; };
struct Call  : public Expression { void print() const; };




};

#endif
