#pragma once

#include "node.h"
#include "enums.h"

class CsgOpNode : public AbstractNode
{
public:
	VISITABLE();
	OpenSCADOperator type;
	CsgOpNode(const ModuleInstantiation *mi, OpenSCADOperator type) : AbstractNode(mi), type(type) { }
	virtual std::string toString() const override;
	virtual std::string name() const override;
};
