#pragma once

#include "ASTDefines.h"

namespace redji {
	
	class SyntaxVisitor {
	public:
		// Default
		virtual void visit(SyntaxNode &node) { };

		// File
		virtual void visit(CompilationUnit &node) { };
		virtual void visit(ClassSyntax &node) { };
		virtual void visit(FunctionSyntax &node) { };
		virtual void visit(MemberSyntax &node) { };
		virtual void visit(ParameterSyntax &node) { };
		virtual void visit(TypeNameSyntax &node) { };
		virtual void visit(TypeSyntax &node) { };

		// Functions
		virtual void visit(LocalSyntax &node) { };
		virtual void visit(BlockSyntax &node) { };
		virtual void visit(StatmentSyntax &node) { };
		virtual void visit(ExpressionStatementSyntax &node) { };
		virtual void visit(ReturnSyntax &node) { };

		// Expressions
		virtual void visit(ExpressionSyntax &node) { };
		virtual void visit(LookupSyntax &node) { };
		virtual void visit(GenericInitializationSyntax &node) { };
		virtual void visit(InvokeSyntax &node) { };
		virtual void visit(NewSyntax &node) { };
		virtual void visit(PrefixOperatorSyntax &node) { };
		virtual void visit(AssignmentSyntax &node) { };
		virtual void visit(OperatorSyntax &node) { };
		virtual void visit(ListSyntax &node) { };
		virtual void visit(IdentifierSyntax &node) { };
		virtual void visit(LiteralSyntax &node) { };
		virtual void visit(BracketSyntax &node) { };
	};
}