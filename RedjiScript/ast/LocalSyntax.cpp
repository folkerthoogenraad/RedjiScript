#include "LocalSyntax.h"

void redji::LocalSyntax::toString(std::ostream & stream) const
{
	stream << "var ";
	stream << m_Name;

	if (m_Type != nullptr) {
		stream << " : ";
		m_Type->toString(stream);
	}

	if (m_InitialValue != nullptr) {
		stream << " = ";
		m_InitialValue->toString(stream);
	}

	stream << ";";
}
