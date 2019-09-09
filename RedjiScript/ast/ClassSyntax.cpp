#include "ClassSyntax.h"

void redji::ClassSyntax::toString(std::ostream & stream) const
{
	// TODO make this depend on the actual type.
	stream << "struct ";
	m_Name.toString(stream);

	stream << "{";

	for (auto &mem : m_Members)
		mem->toString(stream);

	stream << "}";
}
