#include "MemberSyntax.h"

void redji::MemberSyntax::toString(std::ostream & stream) const
{
	stream << m_Name;
	stream << " : ";
	m_Type->toString(stream);

	if (m_InitialValue != nullptr) {
		stream << " = ";
		m_InitialValue->toString(stream);
	}

	stream << ";";
}
