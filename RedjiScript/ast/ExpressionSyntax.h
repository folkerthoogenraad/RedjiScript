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

		void accept(SyntaxVisitor &visitor) override { visitor.visit(*this); }
		virtual void toString(std::ostream &stream) const { stream << "{ERROR}"; };
	};

	class LookupSyntax : public ExpressionSyntax {
	public:
		std::shared_ptr<ExpressionSyntax> m_Lhs;
		std::shared_ptr<ExpressionSyntax> m_Rhs_;
		std::string m_Name;

		void accept(SyntaxVisitor &visitor) override { visitor.visit(*this); }
		virtual void toString(std::ostream &stream) const;
	};

	class GenericInitializeSyntax : public ExpressionSyntax {
	public:
		std::shared_ptr<ExpressionSyntax> m_Lhs;
		std::vector<std::shared_ptr<TypeSyntax>> m_GenericTypes;

		void accept(SyntaxVisitor &visitor) override { visitor.visit(*this); }
		virtual void toString(std::ostream &stream) const;
	};

	class InvokeSyntax : public ExpressionSyntax {
	public:
		std::shared_ptr<ExpressionSyntax> m_Lhs;
		std::vector<std::shared_ptr<ExpressionSyntax>> m_Arguments; // List or single argument

		void accept(SyntaxVisitor &visitor) override { visitor.visit(*this); }
		virtual void toString(std::ostream &stream) const;
	};

	class PrefixOperatorSyntax : public ExpressionSyntax {
	public:
		std::shared_ptr<ExpressionSyntax> m_Lhs;

		enum {// +harry, -harry, !harry
			Plus, Minus, Not
		} m_Operator = Plus;

		std::shared_ptr<ExpressionSyntax> m_Rhs;

		void accept(SyntaxVisitor &visitor) override { visitor.visit(*this); }
		virtual void toString(std::ostream &stream) const;
	};

	class AssignmentSyntax : public ExpressionSyntax {
	public:
		std::shared_ptr<ExpressionSyntax> m_Lhs;
		
		enum {// =, +=, -=, /=, *=, %=
			Normal, Add, Subtract, Divide, Multiply, Modulo
		} m_Type = Normal;

		std::shared_ptr<ExpressionSyntax> m_Rhs;

		void accept(SyntaxVisitor &visitor) override { visitor.visit(*this); }
		virtual void toString(std::ostream &stream) const;
	};

	class OperatorSyntax : public ExpressionSyntax {
	public:
		std::shared_ptr<ExpressionSyntax> m_Lhs;
		enum { // +, -, *, /, %
			Add, Subtract, Multiply, Divide, Modulo
		}m_Operator;
		std::shared_ptr<ExpressionSyntax> m_Rhs;

		void accept(SyntaxVisitor &visitor) override { visitor.visit(*this); }
		virtual void toString(std::ostream &stream) const;
	};

	class ListSyntax : public ExpressionSyntax {
	public:
		std::vector<std::shared_ptr<ExpressionSyntax>> m_List;

		void accept(SyntaxVisitor &visitor) override { visitor.visit(*this); }
		virtual void toString(std::ostream &stream) const;
	};

	class IdentifierSyntax : public ExpressionSyntax {
	public:
		std::string m_Name;

		void accept(SyntaxVisitor &visitor) override { visitor.visit(*this); }
		virtual void toString(std::ostream &stream) const;
	};

	class NewSyntax : public ExpressionSyntax {
	public:
		std::shared_ptr<TypeSyntax> m_Type;

		void accept(SyntaxVisitor &visitor) override { visitor.visit(*this); }
		virtual void toString(std::ostream &stream) const;
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

		void accept(SyntaxVisitor &visitor) override { visitor.visit(*this); }
		virtual void toString(std::ostream &stream) const;
	};

	class BracketSyntax : public ExpressionSyntax {
	public:
		std::shared_ptr<ExpressionSyntax> m_Expression;

		void accept(SyntaxVisitor &visitor) override { visitor.visit(*this); }
		virtual void toString(std::ostream &stream) const;
	};
}