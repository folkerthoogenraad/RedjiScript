#pragma once

#include <string>
#include <vector>

#include "TypeSyntax.h"

namespace redji {

	class ExpressionSyntax {
	public:
		Token m_Token;

		virtual ~ExpressionSyntax() {};
	};

	class LookupSyntax : public ExpressionSyntax {
	public:
		std::shared_ptr<ExpressionSyntax> m_Instance;

		// TODO this should be just a string, because this you can't do harry.(gerrit.hans), only (harry.gerrit).hans
		// std::shared_ptr<ExpressionSyntax> m_Rhs; 
		std::string m_Name;
	};

	class GenericInitializeExpression : public ExpressionSyntax {
	public:
		std::shared_ptr<ExpressionSyntax> m_Lhs;
		std::vector<TypeSyntax> m_GenericTypes;
	};

	class InvokeSyntax : public ExpressionSyntax {
	public:
		std::shared_ptr<ExpressionSyntax> m_Lhs;		// Object to invoke on
		std::optional<std::string> m_Name;				// Name of the function
		std::shared_ptr<ExpressionSyntax> m_Arguments;	// List or single argument
	};

	class PrefixOperatorSyntax : public ExpressionSyntax {
	public:
		std::shared_ptr<ExpressionSyntax> m_Lhs;

		enum {// +harry, -harry, !harry
			Plus, Minus, Not
		} m_Operator = Plus;

		std::shared_ptr<ExpressionSyntax> m_Rhs;
	};

	class AssignmentSyntax : public ExpressionSyntax {
	public:
		std::shared_ptr<ExpressionSyntax> m_Lhs;
		
		enum {// =, +=, -=, /=, *=, %=
			Normal, Add, Subtract, Divide, Multiply, Modulo
		} m_Type = Normal;

		std::shared_ptr<ExpressionSyntax> m_Rhs;
	};

	class OperatorSyntax : public ExpressionSyntax {
	public:
		std::shared_ptr<ExpressionSyntax> m_Lhs;
		enum { // +, -, *, /, %
			Add, Subtract, Multiply, Divide, Modulo
		}m_Operator;
		std::shared_ptr<ExpressionSyntax> m_Rhs;
	};

	class ListSyntax : public ExpressionSyntax {
	public:
		std::vector<std::shared_ptr<ExpressionSyntax>> m_List;
	};

	class IdentifierSyntax : public ExpressionSyntax{
	public:
		std::string m_Name;
	};

	class LiteralSyntax : public ExpressionSyntax {
	public:
		enum Type {
			Unknown,
			Float,
			Double,
			Integer,
			Long,
			String
		} m_Type = Unknown;

		std::string m_Data;
	};

}