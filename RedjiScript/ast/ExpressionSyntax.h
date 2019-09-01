#pragma once

#include <string>
#include <vector>

#include "TypeSyntax.h"
#include "SyntaxNode.h"

namespace redji {

	class ExpressionSyntax : public SyntaxNode{
	public:
		Token m_Token;

		virtual ~ExpressionSyntax() {};
	};

	class LookupSyntax : public ExpressionSyntax {
	public:
		std::shared_ptr<ExpressionSyntax> m_Lhs;
		std::shared_ptr<ExpressionSyntax> m_Rhs; // List or single argument
	};

	class GenericInitializeExpression : public ExpressionSyntax {
	public:
		std::shared_ptr<ExpressionSyntax> m_Lhs;
		std::vector<std::shared_ptr<TypeSyntax>> m_GenericTypes;
	};

	class InvokeSyntax : public ExpressionSyntax {
	public:
		std::shared_ptr<ExpressionSyntax> m_Lhs;
		std::shared_ptr<ExpressionSyntax> m_Arguments; // List or single argument
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