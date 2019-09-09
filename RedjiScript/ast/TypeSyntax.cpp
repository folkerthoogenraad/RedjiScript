#include "TypeSyntax.h"

namespace redji {
	void TypeSyntax::toString(std::ostream & stream) const
	{
		stream << m_Name;

		if (m_Generics.size() > 0) {
			stream << "<";
			
			for (int i = 0; i < m_Generics.size(); i++) {
				m_Generics[i]->toString(stream);

				if(i != m_Generics.size() - 1)
					stream << ",";
			}

			stream << ">";
		}

		for (auto &arr : m_Arrays) {
			stream << "[";
			if (arr.type == Array::Stack) {
				stream << arr.size;
			}
			stream << "[";
		}
	}
}
