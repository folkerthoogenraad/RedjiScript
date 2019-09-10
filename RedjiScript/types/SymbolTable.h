#pragma once

#include <vector>
#include <map>
#include <memory>

#include "Symbol.h"
#include "logger/Logger.h"

#include "ast/AST.h"

namespace redji {

	class SymbolTable {
		std::shared_ptr<NamespaceSymbol> m_RootNamespace;

		std::map<SyntaxNode*, std::shared_ptr<Symbol>> m_Symbols;

		Logger logger;
	public:
		SymbolTable();

		void process(std::shared_ptr<CompilationUnit> unit);

		void insertSymbol(SyntaxNode* node, std::shared_ptr<Symbol> sym);
		std::shared_ptr<Symbol> findSymbol(SyntaxNode* node);
	private:
		std::shared_ptr<TypeSymbol> createTypeSymbol(std::shared_ptr<TypeSyntax> syntax);

		void processFunction(std::shared_ptr<FunctionSymbol> func);
		void processStatement(std::shared_ptr<SymbolScope> scope, std::shared_ptr<StatementSyntax> statement);
		void processExpression(std::shared_ptr<SymbolScope> scope, std::shared_ptr<ExpressionSyntax> expression);
	};
}