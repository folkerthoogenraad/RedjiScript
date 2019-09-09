#include "ParameterSyntax.h"

void redji::ParameterSyntax::toString(std::ostream & stream) const
{
	stream << m_Name;
	stream << " : ";
	m_Type->toString(stream);
}
