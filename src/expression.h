#pragma once

#include "AST.h"

#include <string>
#include <vector>
#include "value.h"
#include "memory.h"
#include "Assignment.h"

class Expression : public ASTNode
{
public:
	Expression(Location loc) : ASTNode(loc) {}
	virtual ~Expression() {}
	virtual bool isLiteral() const;
	virtual ValuePtr evaluate(const class Context *context) const = 0;
	virtual void print(std::ostream &stream) const = 0;
};

std::ostream &operator<<(std::ostream &stream, const Expression &expr);

class UnaryOp : public Expression
{
public:
	enum class Op {
		Not,
		Negate
	};
	UnaryOp(Op op, Expression *expr, Location loc) : Expression(loc), op(op), expr(expr) {}

	virtual bool isLiteral() const override;
	virtual ValuePtr evaluate(const class Context *context) const override;
	virtual void print(std::ostream &stream) const override;

private:
	const std::string opString() const;

	Op op;
	shared_ptr<Expression> expr;
};

class BinaryOp : public Expression
{
public:
	enum class Op {
		LogicalAnd,
		LogicalOr,
		Multiply,
		Divide,
		Modulo,
		Plus,
		Minus,
		Less,
		LessEqual,
		Greater,
		GreaterEqual,
		Equal,
		NotEqual
	};

	BinaryOp(Expression *left, Op op, Expression *right, Location loc)
		: Expression(loc), op(op), left(left), right(right)
	{
	}

	virtual ValuePtr evaluate(const class Context *context) const override;
	virtual void print(std::ostream &stream) const override;

private:
	const std::string opString() const;

	Op op;
	shared_ptr<Expression> left;
	shared_ptr<Expression> right;
};

class TernaryOp : public Expression
{
public:
	TernaryOp(Expression *cond, Expression *ifexpr, Expression *elseexpr, Location loc)
		: Expression(loc), cond(cond), ifexpr(ifexpr), elseexpr(elseexpr)
	{
	}

	virtual ValuePtr evaluate(const class Context *context) const override;
	virtual void print(std::ostream &stream) const override;

	shared_ptr<Expression> cond;
	shared_ptr<Expression> ifexpr;
	shared_ptr<Expression> elseexpr;
};

class ArrayLookup : public Expression
{
public:
	ArrayLookup(Expression *array, Expression *index, Location loc)
		: Expression(loc), array(array), index(index)
	{
	}

	virtual ValuePtr evaluate(const class Context *context) const override;
	virtual void print(std::ostream &stream) const override;
private:
	shared_ptr<Expression> array;
	shared_ptr<Expression> index;
};

class Literal : public Expression
{
public:
	Literal(const ValuePtr &val, Location loc = Location::NONE) : Expression(loc), value(val)
	{
	}

	virtual ValuePtr evaluate(const class Context *) const override;
	virtual void print(std::ostream &stream) const override;
	virtual bool isLiteral() const override { return true; }
private:
	ValuePtr value;
};

class Range : public Expression
{
public:
	Range(Expression *begin, Expression *end, Location loc)
		: Expression(loc), begin(begin), end(end)
	{
	}

	Range(Expression *begin, Expression *step, Expression *end, Location loc)
		: Expression(loc), begin(begin), step(step), end(end)
	{
	}

	virtual ValuePtr evaluate(const class Context *context) const override;
	virtual void print(std::ostream &stream) const override;
	virtual bool isLiteral() const override;
private:
	shared_ptr<Expression> begin;
	shared_ptr<Expression> step;
	shared_ptr<Expression> end;
};

class Vector : public Expression
{
public:
	Vector(Location loc) : Expression(loc) {}

	virtual ValuePtr evaluate(const class Context *context) const override;
	virtual void print(std::ostream &stream) const override;
	virtual bool isLiteral() const override;

	void push_back(Expression *expr);
private:
	std::vector<shared_ptr<Expression>> children;
};

class Lookup : public Expression
{
public:
	Lookup(const std::string &name, Location loc) : Expression(loc), name(name) {}

	virtual ValuePtr evaluate(const class Context *context) const override;
	virtual void print(std::ostream &stream) const override;
private:
	std::string name;
};

class MemberLookup : public Expression
{
public:
	MemberLookup(Expression *expr, const std::string &member, Location loc)
		: Expression(loc), expr(expr), member(member)
	{
	}

	virtual ValuePtr evaluate(const class Context *context) const override;
	virtual void print(std::ostream &stream) const override;
private:
	shared_ptr<Expression> expr;
	std::string member;
};

class FunctionCall : public Expression
{
public:
	FunctionCall(const std::string &funcname, const AssignmentList &arglist, Location loc)
		: Expression(loc), name(funcname), arguments(arglist)
	{
	}
	virtual ValuePtr evaluate(const class Context *context) const override;
	virtual void print(std::ostream &stream) const override;
	static Expression *create(const std::string &funcname, const AssignmentList &arglist, Expression *expr, Location loc);
public:
	std::string name;
	AssignmentList arguments;
};

class Assert : public Expression
{
public:
	Assert(const AssignmentList &args, Expression *expr, Location loc)
		: Expression(loc), arguments(args), expr(expr)
	{
	}

	virtual ValuePtr evaluate(const class Context *context) const override;
	virtual void print(std::ostream &stream) const override;
private:
	AssignmentList arguments;
	shared_ptr<Expression> expr;
};

class Echo : public Expression
{
public:
	Echo(const AssignmentList &args, Expression *expr, Location loc)
	: Expression(loc), arguments(args), expr(expr)
{

}
	virtual ValuePtr evaluate(const class Context *context) const override;
	virtual void print(std::ostream &stream) const override;
private:
	AssignmentList arguments;
	shared_ptr<Expression> expr;
};

class Let : public Expression
{
public:
	Let(const AssignmentList &args, Expression *expr, Location loc)
	: Expression(loc), arguments(args), expr(expr)
{
}
	virtual ValuePtr evaluate(const class Context *context) const override;
	virtual void print(std::ostream &stream) const override;
private:
	AssignmentList arguments;
	shared_ptr<Expression> expr;
};

class ListComprehension : public Expression
{
public:
	ListComprehension(Location loc)
	: Expression(loc) {}
	~ListComprehension() = default;
};

class LcIf : public ListComprehension
{
public:
	LcIf(Expression *cond, Expression *ifexpr, Expression *elseexpr, Location loc)
		: ListComprehension(loc), cond(cond), ifexpr(ifexpr), elseexpr(elseexpr) {}
	virtual ValuePtr evaluate(const class Context *context) const override;
	virtual void print(std::ostream &stream) const override;
private:
	shared_ptr<Expression> cond;
	shared_ptr<Expression> ifexpr;
	shared_ptr<Expression> elseexpr;
};

class LcFor : public ListComprehension
{
public:
	LcFor(const AssignmentList &args, Expression *expr, Location loc)
	: ListComprehension(loc), arguments(args), expr(expr) {}
	virtual ValuePtr evaluate(const class Context *context) const override;
	virtual void print(std::ostream &stream) const override;
private:
	AssignmentList arguments;
	shared_ptr<Expression> expr;
};

class LcForC : public ListComprehension
{
public:
	LcForC(const AssignmentList &args, const AssignmentList &incrargs, Expression *cond, Expression *expr, Location loc)
	: ListComprehension(loc), arguments(args), incr_arguments(incrargs), cond(cond), expr(expr) {}
	virtual ValuePtr evaluate(const class Context *context) const override;
	virtual void print(std::ostream &stream) const override;
private:
	AssignmentList arguments;
	AssignmentList incr_arguments;
	shared_ptr<Expression> cond;
	shared_ptr<Expression> expr;
};

class LcEach : public ListComprehension
{
public:
	LcEach(Expression *expr, Location loc)
	: ListComprehension(loc), expr(expr) {}
	virtual ValuePtr evaluate(const class Context *context) const override;
	virtual void print(std::ostream &stream) const override;
private:
	shared_ptr<Expression> expr;
};

class LcLet : public ListComprehension
{
public:
	LcLet(const AssignmentList &args, Expression *expr, Location loc)
		: ListComprehension(loc), arguments(args), expr(expr) {}
	virtual ValuePtr evaluate(const class Context *context) const override;
	virtual void print(std::ostream &stream) const override;
private:
	AssignmentList arguments;
	shared_ptr<Expression> expr;
};

void evaluate_assert(const Context &context, const class EvalContext *evalctx, const Location &loc);
