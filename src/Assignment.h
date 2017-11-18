#pragma once

#include <string>
#include <vector>
#include <utility>

#include "value.h"
#include "AST.h"
#include "memory.h"
#include "annotation.h"

class Assignment : public ASTNode
{
public:
	Assignment(std::string name, Location loc) : ASTNode(loc), name(name) {}

	Assignment(std::string name, shared_ptr<class Expression> expr = shared_ptr<class Expression>(),
						 Location loc = Location::NONE)
		: ASTNode(loc), name(name), expr(expr)
	{
	}

	std::string name;
	shared_ptr<class Expression> expr;

	virtual void addAnnotations(AnnotationList *annotations);
	virtual bool hasAnnotations() const;
	virtual const Annotation *annotation(const std::string &name) const;

protected:
	AnnotationMap annotations;
};


using AssignmentList = std::vector<Assignment>;
using AssignmentMap = std::unordered_map<std::string, const Expression *>;
