#pragma once

#include <string>

#include "context.h"

class BuiltinContext : public Context
{
public:
	BuiltinContext();
	virtual ~BuiltinContext() {}

	virtual ValuePtr evaluate_function(const std::string &name, const class EvalContext *evalctx) const;
	virtual class AbstractNode *instantiate_module(const class ModuleInstantiation &inst, EvalContext *evalctx) const;
};
