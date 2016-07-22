#include "Assignment.h"
#include "expression.h"
#include <sstream>

void Assignment::print(std::ostream &stream, const std::string &indent) const
{
	stream << indent << this->name << " = " << *this->expr << ";\n";
}
