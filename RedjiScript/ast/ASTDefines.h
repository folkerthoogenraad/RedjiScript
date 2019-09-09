#pragma once

namespace redji {
	class CompilationUnit;

	class SyntaxNode;
	
	class ClassSyntax;
	class FunctionSyntax;
	class MemberSyntax;
	class ParameterSyntax;
	class TypeNameSyntax;
	class TypeSyntax;

	// Within functions
	class LocalSyntax;
	class BlockSyntax;
	class StatmentSyntax;
	class ExpressionStatementSyntax;
	class ReturnSyntax;

	// Expressions
	class ExpressionSyntax;
	class LookupSyntax;
	class GenericInitializationSyntax;
	class InvokeSyntax;
	class PrefixOperatorSyntax;
	class AssignmentSyntax;
	class OperatorSyntax;
	class ListSyntax;
	class IdentifierSyntax;
	class LiteralSyntax;
	class BracketSyntax;
}