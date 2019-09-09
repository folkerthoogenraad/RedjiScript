#include "ReturnSyntax.h"

void redji::ReturnSyntax::toString(std::ostream & stream) const
{
	stream << "return ";
	m_Body->toString(stream);
	stream << ";";
}
