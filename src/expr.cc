/*
 *  OpenSCAD (www.openscad.org)
 *  Copyright (C) 2009-2011 Clifford Wolf <clifford@clifford.at> and
 *                          Marius Kintel <marius@kintel.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  As a special exception, you have permission to link this program
 *  with the CGAL library and distribute executables, as long as you
 *  follow the requirements of the GNU GPL in regard to all of the
 *  software in the executable aside from CGAL.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
#include "expression.h"

#include <cstdint>
#include <assert.h>
#include <sstream>
#include <algorithm>

#include "value.h"
#include "evalcontext.h"
#include "printutils.h"
#include "stackcheck.h"
#include "exceptions.h"
#include "feature.h"

namespace {

bool isListComprehension(const shared_ptr<Expression> &e)
{
	return dynamic_cast<const ListComprehension *>(e.get());
}

Value::VectorType flatten(Value::VectorType const &vec)
{
	int n = 0;
	for (const auto &value : vec) {
		assert(value->type() == Value::ValueType::VECTOR);
		n += value->toVector().size();
	}
	Value::VectorType ret;
	ret.reserve(n);
	for (const auto &value : vec) {
		const auto vectorValue = value->toVector();
		std::copy(vectorValue.begin(), vectorValue.end(), std::back_inserter(ret));
	}
	return ret;
}

void evaluate_sequential_assignment(const AssignmentList &assignment_list, Context *context)
{
	EvalContext ctx{context, assignment_list};
	ctx.assignTo(*context);
}

std::ostream &operator<<(std::ostream &o, AssignmentList const &l) {
	for (auto i = 0u; i < l.size(); i++) {
		const auto &arg = l[i];
		if (i > 0) o << ", ";
		if (!arg.name.empty()) o << arg.name << " = ";
		o << *arg.expr;
	}
	return o;
}

}

bool Expression::isLiteral() const
{
	return false;
}

ValuePtr UnaryOp::evaluate(const Context *context) const
{
	switch (this->op) {
	case (Op::Not):
		return !this->expr->evaluate(context);
	case (Op::Negate):
		return -this->expr->evaluate(context);
	default:
		return ValuePtr::undefined;
		// FIXME: error:
	}
}

const std::string UnaryOp::opString() const
{
	switch (this->op) {
	case Op::Not:
		return "!";
		break;
	case Op::Negate:
		return "-";
		break;
	default:
		return "";
		// FIXME: Error: unknown op
	}
}

bool UnaryOp::isLiteral() const
{
	return this->expr->isLiteral();
}

void UnaryOp::print(std::ostream &stream) const
{
	stream << opString() << *this->expr;
}

ValuePtr BinaryOp::evaluate(const Context *context) const
{
	switch (this->op) {
	case Op::LogicalAnd:
		return this->left->evaluate(context) && this->right->evaluate(context);
		break;
	case Op::LogicalOr:
		return this->left->evaluate(context) || this->right->evaluate(context);
		break;
	case Op::Multiply:
		return this->left->evaluate(context) * this->right->evaluate(context);
		break;
	case Op::Divide:
		return this->left->evaluate(context) / this->right->evaluate(context);
		break;
	case Op::Modulo:
		return this->left->evaluate(context) % this->right->evaluate(context);
		break;
	case Op::Plus:
		return this->left->evaluate(context) + this->right->evaluate(context);
		break;
	case Op::Minus:
		return this->left->evaluate(context) - this->right->evaluate(context);
		break;
	case Op::Less:
		return this->left->evaluate(context) < this->right->evaluate(context);
		break;
	case Op::LessEqual:
		return this->left->evaluate(context) <= this->right->evaluate(context);
		break;
	case Op::Greater:
		return this->left->evaluate(context) > this->right->evaluate(context);
		break;
	case Op::GreaterEqual:
		return this->left->evaluate(context) >= this->right->evaluate(context);
		break;
	case Op::Equal:
		return this->left->evaluate(context) == this->right->evaluate(context);
		break;
	case Op::NotEqual:
		return this->left->evaluate(context) != this->right->evaluate(context);
		break;
	default:
		return ValuePtr::undefined;
		// FIXME: Error: unknown op
	}
}

const std::string BinaryOp::opString() const
{
	switch (this->op) {
	case Op::LogicalAnd:
		return "&&";
		break;
	case Op::LogicalOr:
		return "||";
		break;
	case Op::Multiply:
		return "*";
		break;
	case Op::Divide:
		return "/";
		break;
	case Op::Modulo:
		return "%";
		break;
	case Op::Plus:
		return "+";
		break;
	case Op::Minus:
		return "-";
		break;
	case Op::Less:
		return "<";
		break;
	case Op::LessEqual:
		return "<=";
		break;
	case Op::Greater:
		return ">";
		break;
	case Op::GreaterEqual:
		return ">=";
		break;
	case Op::Equal:
		return "==";
		break;
	case Op::NotEqual:
		return "!=";
		break;
	default:
		return "";
		// FIXME: Error: unknown op
	}
}

void BinaryOp::print(std::ostream &stream) const
{
	stream << "(" << *this->left << " " << opString() << " " << *this->right << ")";
}

ValuePtr TernaryOp::evaluate(const Context *context) const
{
	return (this->cond->evaluate(context) ? this->ifexpr : this->elseexpr)->evaluate(context);
}

void TernaryOp::print(std::ostream &stream) const
{
	stream << "(" << *this->cond << " ? " << *this->ifexpr << " : " << *this->elseexpr << ")";
}

ValuePtr ArrayLookup::evaluate(const Context *context) const
{
	return this->array->evaluate(context)[this->index->evaluate(context)];
}

void ArrayLookup::print(std::ostream &stream) const
{
	stream << *array << "[" << *index << "]";
}

ValuePtr Literal::evaluate(const class Context *) const
{
	return this->value;
}

void Literal::print(std::ostream &stream) const
{
	stream << *this->value;
}

ValuePtr Range::evaluate(const Context *context) const
{
	const auto beginValue = this->begin->evaluate(context);
	if (beginValue->type() == Value::ValueType::NUMBER) {
		const auto endValue = this->end->evaluate(context);
		if (endValue->type() == Value::ValueType::NUMBER) {
			if (!this->step) {
				RangeType range{beginValue->toDouble(), endValue->toDouble()};
				return {range};
			}
			else {
				const auto stepValue = this->step->evaluate(context);
				if (stepValue->type() == Value::ValueType::NUMBER) {
					RangeType range{beginValue->toDouble(), stepValue->toDouble(), endValue->toDouble()};
					return ValuePtr(range);
				}
			}
		}
	}
	return ValuePtr::undefined;
}

void Range::print(std::ostream &stream) const
{
	stream << "[" << *this->begin;
	if (this->step) stream << " : " << *this->step;
	stream << " : " << *this->end;
	stream << "]";
}

bool Range::isLiteral() const
{
	return begin->isLiteral() && end->isLiteral() && (!this->step || step->isLiteral());
}

bool Vector::isLiteral() const
{
	return std::all_of(this->children.cbegin(), this->children.cend(),
										 [](const shared_ptr<Expression> &expr) { return expr->isLiteral(); });
}

void Vector::push_back(Expression *expr)
{
	this->children.push_back(shared_ptr<Expression>(expr));
}

ValuePtr Vector::evaluate(const Context *context) const
{
	Value::VectorType vec;
	for (const auto &e : this->children) {
		auto tmpval = e->evaluate(context);
		if (isListComprehension(e)) {
			for (const auto &value : tmpval->toVector()) {
				vec.push_back(value);
			}
		}
		else {
			vec.push_back(tmpval);
		}
	}
	return {vec};
}

void Vector::print(std::ostream &stream) const
{
	stream << "[";
	for (size_t i = 0; i < this->children.size(); i++) {
		if (i > 0) stream << ", ";
		stream << *this->children[i];
	}
	stream << "]";
}

ValuePtr Lookup::evaluate(const Context *context) const
{
	return context->lookup_variable(this->name);
}

void Lookup::print(std::ostream &stream) const
{
	stream << this->name;
}

ValuePtr MemberLookup::evaluate(const Context *context) const
{
	const auto v = this->expr->evaluate(context);

	if (v->type() == Value::ValueType::VECTOR) {
		if (this->member == "x") return v[0];
		if (this->member == "y") return v[1];
		if (this->member == "z") return v[2];
	}
	else if (v->type() == Value::ValueType::RANGE) {
		if (this->member == "begin") return v[0];
		if (this->member == "step") return v[1];
		if (this->member == "end") return v[2];
	}
	return ValuePtr::undefined;
}

void MemberLookup::print(std::ostream &stream) const
{
	stream << *this->expr << "." << this->member;
}

ValuePtr FunctionCall::evaluate(const Context *context) const
{
	if (StackCheck::inst()->check()) {
		throw RecursionException::create("function", this->name);
	}

	EvalContext c{context, this->arguments};
	return context->evaluate_function(this->name, &c);
}

void FunctionCall::print(std::ostream &stream) const
{
	stream << this->name << "(" << this->arguments << ")";
}

Expression *FunctionCall::create(const std::string &funcname, const AssignmentList &arglist, Expression *expr, Location loc)
{
	if (funcname == "assert" && Feature::ExperimentalAssertExpression.is_enabled()) {
		return new Assert(arglist, expr, loc);
	}
	else if (funcname == "echo" && Feature::ExperimentalEchoExpression.is_enabled()) {
		return new Echo(arglist, expr, loc);
	}
	else if (funcname == "let") {
		return new Let(arglist, expr, loc);
	}

	// TODO: Generate error/warning if expr != 0?
	return new FunctionCall(funcname, arglist, loc);
}

ValuePtr Assert::evaluate(const Context *context) const
{
	EvalContext assert_context{context, this->arguments};

	Context c{&assert_context};
	evaluate_assert(c, &assert_context, loc);

	return expr ? expr->evaluate(&c) : ValuePtr::undefined;
}

void Assert::print(std::ostream &stream) const
{
	stream << "assert(" << this->arguments << ")";
	if (this->expr) stream << " " << *this->expr;
}

ValuePtr Echo::evaluate(const Context *context) const
{
	ExperimentalFeatureException::check(Feature::ExperimentalEchoExpression);

	std::stringstream msg;
	EvalContext echo_context{context, this->arguments};
	msg << "ECHO: " << echo_context;
	PRINTB("%s", msg.str());

	return expr ? expr->evaluate(context) : ValuePtr::undefined;
}

void Echo::print(std::ostream &stream) const
{
	stream << "echo(" << this->arguments << ")";
	if (this->expr) stream << " " << *this->expr;
}

ValuePtr Let::evaluate(const Context *context) const
{
	Context c{context};
	evaluate_sequential_assignment(this->arguments, &c);

	return this->expr->evaluate(&c);
}

void Let::print(std::ostream &stream) const
{
	stream << "let(" << this->arguments << ") " << *expr;
}

ValuePtr LcIf::evaluate(const Context *context) const
{
	if (this->elseexpr) {
		ExperimentalFeatureException::check(Feature::ExperimentalElseExpression);
	}

	const auto &expr = this->cond->evaluate(context) ? this->ifexpr : this->elseexpr;

	Value::VectorType vec;
	if (expr) {
		if (isListComprehension(expr)) {
			return expr->evaluate(context);
		}
		else {
			vec.push_back(expr->evaluate(context));
		}
	}

	return {vec};
}

void LcIf::print(std::ostream &stream) const
{
	stream << "if(" << *this->cond << ") (" << *this->ifexpr << ")";
	if (this->elseexpr) {
		stream << " else (" << *this->elseexpr << ")";
	}
}

ValuePtr LcEach::evaluate(const Context *context) const
{
	ExperimentalFeatureException::check(Feature::ExperimentalEachExpression);

	auto v = this->expr->evaluate(context);

	Value::VectorType vec;
	if (v->type() == Value::ValueType::RANGE) {
		auto range = v->toRange();
		auto steps = range.numValues();
		if (steps >= 1000000) {
			PRINTB("WARNING: Bad range parameter in for statement: too many elements (%lu).", steps);
		}
		else {
			for (const auto value : range) {
				vec.push_back(ValuePtr(value));
			}
		}
	}
	else if (v->type() == Value::ValueType::VECTOR) {
		for (const auto &value : v->toVector()) {
			vec.push_back(value);
		}
	}
	else if (v->type() != Value::ValueType::UNDEFINED) {
		vec.push_back(v);
	}

	return {isListComprehension(this->expr) ? flatten(vec) : vec};
}

void LcEach::print(std::ostream &stream) const
{
	stream << "each (" << *this->expr << ")";
}

ValuePtr LcFor::evaluate(const Context *context) const
{
	EvalContext for_context{context, this->arguments};

	Context assign_context{context};

	// comprehension for statements are by the parser reduced to only contain one single element
	const auto &it_name = for_context.getArgName(0);
	auto it_values = for_context.getArgValue(0, &assign_context);

	Context c{context};

	Value::VectorType vec;
	if (it_values->type() == Value::ValueType::RANGE) {
		auto range = it_values->toRange();
		auto steps = range.numValues();
		if (steps >= 1000000) {
			PRINTB("WARNING: Bad range parameter in for statement: too many elements (%lu).", steps);
		}
		else {
			for (const auto value : range) {
				c.set_variable(it_name, ValuePtr(value));
				vec.push_back(this->expr->evaluate(&c));
			}
		}
	}
	else if (it_values->type() == Value::ValueType::VECTOR) {
		for (const auto &val : it_values->toVector()) {
			c.set_variable(it_name, val);
			vec.push_back(this->expr->evaluate(&c));
		}
	}
	else if (it_values->type() != Value::ValueType::UNDEFINED) {
		c.set_variable(it_name, it_values);
		vec.push_back(this->expr->evaluate(&c));
	}

	return {isListComprehension(this->expr) ? flatten(vec) : vec};
}

void LcFor::print(std::ostream &stream) const
{
	stream << "for(" << this->arguments << ") (" << *this->expr << ")";
}

ValuePtr LcForC::evaluate(const Context *context) const
{
	ExperimentalFeatureException::check(Feature::ExperimentalForCExpression);

	Context c{context};
	evaluate_sequential_assignment(this->arguments, &c);

	unsigned int counter = 0;
	Value::VectorType vec;
	while (this->cond->evaluate(&c)) {
		vec.push_back(this->expr->evaluate(&c));

		if (counter++ == 1000000) throw RecursionException::create("for loop", "");

		Context tmp{&c};
		evaluate_sequential_assignment(this->incr_arguments, &tmp);
		c.apply_variables(tmp);
	}

	return {isListComprehension(this->expr) ? flatten(vec) : vec};
}

void LcForC::print(std::ostream &stream) const
{
	stream
		<< "for(" << this->arguments
		<< ";" << *this->cond
		<< ";" << this->incr_arguments
		<< ") " << *this->expr;
}

ValuePtr LcLet::evaluate(const Context *context) const
{
	Context c{context};
	evaluate_sequential_assignment(this->arguments, &c);
	return this->expr->evaluate(&c);
}

void LcLet::print(std::ostream &stream) const
{
	stream << "let(" << this->arguments << ") (" << *this->expr << ")";
}

std::ostream &operator<<(std::ostream &stream, const Expression &expr)
{
	expr.print(stream);
	return stream;
}

void evaluate_assert(const Context &context, const class EvalContext *evalctx, const Location &loc)
{
	ExperimentalFeatureException::check(Feature::ExperimentalAssertExpression);

	AssignmentList args{{"condition"}, {"message"}};

	Context c{&context};

	const auto &assignments = evalctx->resolveArguments(args);
	for (const auto &arg : args) {
		auto it = assignments.find(arg.name);
		if (it != assignments.end()) {
			c.set_variable(arg.name, assignments.at(arg.name)->evaluate(evalctx));
		}
	}

	const auto condition = c.lookup_variable("condition");

	if (!condition->toBool()) {
		std::stringstream msg;
		msg << "ERROR: Assertion";

		const Expression *expr = assignments.at("condition");
		if (expr) msg << " '" << *expr << "'";

		msg << " failed, line " << loc.firstLine();

		const auto &message = c.lookup_variable("message", true);
		if (message->isDefined()) msg << ": " << message->toEchoString();

		throw AssertionFailedException(msg.str());
	}
}
