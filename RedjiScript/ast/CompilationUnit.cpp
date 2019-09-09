#include "CompilationUnit.h"

void redji::CompilationUnit::toString(std::ostream & stream) const
{
	for (auto &st : m_Statements)
		st->toString(stream);
}
