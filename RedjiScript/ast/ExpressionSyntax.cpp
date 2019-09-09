#include "ExpressionSyntax.h"

namespace redji {
	void LookupSyntax::toString(std::ostream & stream) const
	{
		m_Lhs->toString(stream);
		stream << ".";
		stream << m_Name;
	}

	void GenericInitializeSyntax::toString(std::ostream & stream) const
	{
		stream << "GenericInitialization<unimplemented>";
	}

	void InvokeSyntax::toString(std::ostream & stream) const
	{
		m_Lhs->toString(stream);
		stream << "(";

		for (int i = 0; i < m_Arguments.size(); i++) {
			m_Arguments[i]->toString(stream);
			if (i != m_Arguments.size() - 1)
				stream << ", ";
		}

		stream << ")";
	}

	void PrefixOperatorSyntax::toString(std::ostream & stream) const
	{
		switch (m_Operator) {
		case Plus:
			stream << "+";
			break;
		case Minus:
			stream << "-";
			break;
		case Not:
			stream << "!";
			break;
		}

		m_Rhs->toString(stream);
	}

	void AssignmentSyntax::toString(std::ostream & stream) const
	{
		m_Lhs->toString(stream);

		switch (m_Type) {
		case Normal: stream << "="; break;
		case Add: stream << "+="; break;
		case Subtract: stream << "-="; break;
		case Multiply: stream << "*="; break;
		case Divide: stream << "/="; break;
		case Modulo: stream << "%="; break;
		}

		m_Rhs->toString(stream);
	}

	void OperatorSyntax::toString(std::ostream & stream) const
	{
		m_Lhs->toString(stream);

		switch (m_Operator) {
		case Add: stream << "+"; break;
		case Subtract: stream << "-"; break;
		case Multiply: stream << "*"; break;
		case Divide: stream << "/"; break;
		case Modulo: stream << "%"; break;
		}

		m_Rhs->toString(stream);
	}

	void ListSyntax::toString(std::ostream & stream) const
	{
		for (int i = 0; i < m_List.size(); i++) {

			if (i != m_List.size() - 1)
				stream << ",";
		}
	}

	void IdentifierSyntax::toString(std::ostream & stream) const
	{
		stream << m_Name;
	}

	void LiteralSyntax::toString(std::ostream & stream) const
	{
		if (m_Type == String)
			stream << "\"";

		stream << m_Data;

		if (m_Type == String)
			stream << "\"";
	}

	void BracketSyntax::toString(std::ostream & stream) const
	{
		stream << "(";
		m_Expression->toString(stream);
		stream << ")";
	}
}
