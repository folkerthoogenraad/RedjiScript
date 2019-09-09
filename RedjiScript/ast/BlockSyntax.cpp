#include "BlockSyntax.h"

void redji::BlockSyntax::toString(std::ostream & stream) const
{
	stream << "{";
	for (auto &st : m_Statements)
		st->toString(stream);
	stream << "}";
}
