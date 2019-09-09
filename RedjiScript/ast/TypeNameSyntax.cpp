#include "TypeNameSyntax.h"

namespace redji {
	void TypeNameSyntax::toString(std::ostream & stream) const
	{
		stream << m_Name;
		if (m_GenericNames.size() > 0) {
			stream << "<";
			for (int i = 0; i < m_GenericNames.size(); i++) {
				stream << m_GenericNames[i];
				if (i != m_GenericNames.size() - 1) {
					stream << ", ";
				}
			}
			stream << ">";
		}
	}
}
