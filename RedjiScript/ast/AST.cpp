#include "AST.h"

namespace redji {
	std::ostream & operator<<(std::ostream & os, const SyntaxNode & dt)
	{
		dt.toString(os);
		return os;
	}
}