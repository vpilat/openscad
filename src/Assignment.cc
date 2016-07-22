#include "Assignment.h"
#include "expression.h"
#include <sstream>

std::string Assignment::dump(const std::string &indent) const
{
	std::stringstream dump;
	dump << indent << this->name << " = " << *this->expr << ";\n";
	return dump.str();
}
