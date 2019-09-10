#include "FunctionSyntax.h"

void redji::FunctionSyntax::toString(std::ostream & stream) const
{
	stream << "function ";
	m_Name.toString(stream);

	stream << "(";

	for (int i = 0; i < m_Parameters.size(); i++) {
		m_Parameters[i]->toString(stream);
		if (i != m_Parameters.size() - 1)
			stream << ", ";
	}

	stream << ")";

	m_Body->toString(stream);
}
