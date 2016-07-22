#pragma once

#include <string>
#include <vector>

#include "AST.h"
#include "memory.h"

class Assignment : public ASTNode
{
public:
	Assignment(std::string name, const Location &loc)
				: ASTNode(loc), name(name) { }
	Assignment(std::string name,
						 shared_ptr<class Expression> expr = shared_ptr<class Expression>(),
						 const Location &loc = Location::NONE)
		: ASTNode(loc), name(name), expr(expr) { }
	
	virtual void print(std::ostream &stream, const std::string &indent) const;
	
	std::string name;
	shared_ptr<class Expression> expr;
};

typedef std::vector<Assignment> AssignmentList;
