#pragma once

#include "node.h"
#include "linalg.h"

class ColorNode : public AbstractNode
{
public:
	VISITABLE();
	ColorNode(const ModuleInstantiation *mi) : AbstractNode(mi), color(-1.0f, -1.0f, -1.0f, 1.0f) { }
	virtual std::string toString() const override;
	virtual std::string name() const override;

	Color4f color;
};
