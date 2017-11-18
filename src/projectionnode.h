#pragma once

#include "node.h"
#include <string>

class ProjectionNode : public AbstractPolyNode
{
public:
	VISITABLE();
	ProjectionNode(const ModuleInstantiation *mi) : AbstractPolyNode(mi) {
		cut_mode = false;
	}
	virtual std::string toString() const override;
	virtual std::string name() const override { return "projection"; }

	int convexity;
	bool cut_mode;
};
