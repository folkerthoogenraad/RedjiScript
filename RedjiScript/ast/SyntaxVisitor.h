#pragma once

#include "ASTDefines.h"

namespace redji {
	
	class SyntaxVisitor {
	public:
		// Default
		void visit(SyntaxNode &node) { };

		// File
		void visit(CompilationUnit &node) { };
		void visit(ClassSyntax &node) { };
		void visit(FunctionSyntax &node) { };
		void visit(MemberSyntax &node) { };
		void visit(ParameterSyntax &node) { };
		void visit(TypeNameSyntax &node) { };
		void visit(TypeSyntax &node) { };

		// Functions
		void visit(LocalSyntax &node) { };
		void visit(BlockSyntax &node) { };
		void visit(StatmentSyntax &node) { };
		void visit(ExpressionStatementSyntax &node) { };
		void visit(ReturnSyntax &node) { };

		// Expressions
		void visit(ExpressionSyntax &node) { };
		void visit(LookupSyntax &node) { };
		void visit(GenericInitializationSyntax &node) { };
		void visit(InvokeSyntax &node) { };
		void visit(PrefixOperatorSyntax &node) { };
		void visit(AssignmentSyntax &node) { };
		void visit(OperatorSyntax &node) { };
		void visit(ListSyntax &node) { };
		void visit(IdentifierSyntax &node) { };
		void visit(LiteralSyntax &node) { };
		void visit(BracketSyntax &node) { };
		void visit(ListSyntax &node) { };
	};
}