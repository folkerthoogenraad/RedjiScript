#include "ExpressionStatementSyntax.h"

void redji::ExpressionStatementSyntax::toString(std::ostream & stream) const
{
	m_Expression->toString(stream);
	stream << ";";
}
