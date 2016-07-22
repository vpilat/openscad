#pragma once

#include <string>

class Location {
public:
	Location(int firstLine, int firstCol, int lastLine, int lastCol)
		: first_line(firstLine), first_col(firstCol), last_line(lastLine), last_col(lastCol) {
	}

	int firstLine() const { return first_line; }
	int firstColumn() const { return first_col; }
	int lastLine() const { return last_line; }
	int lastColumn() const { return last_col; }


	static Location NONE;
private:
	int first_line;
	int first_col;
	int last_line;
	int last_col;
};

class ASTNode
{
public:
  ASTNode(const Location &loc) : loc(loc) {}
	virtual ~ASTNode() {}

	virtual void print(std::ostream &stream, const std::string &indent) const = 0;

	std::string dump(const std::string &indent) const;
	const Location &location() const { return loc; }

protected:
	Location loc;
};

std::ostream &operator<<(std::ostream &stream, const ASTNode &ast);

class UseNode : public ASTNode
{
public:
	UseNode(const std::string &filename, const Location &loc) : ASTNode(loc), filename(filename) {}
	virtual void print(std::ostream &stream, const std::string &indent) const;

	std::string filename;
};

