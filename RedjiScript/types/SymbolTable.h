#pragma once

#include <vector>
#include <map>
#include <memory>

#include "Symbol.h"

#include "ast/AST.h"

namespace redji {

	class SymbolTable {
		std::shared_ptr<NamespaceSymbol> m_RootNamespace;
	public:
		SymbolTable();

		void process(std::shared_ptr<CompilationUnit> unit);

	private:
		std::shared_ptr<TypeSymbol> createTypeSyntax(std::shared_ptr<TypeSyntax> syntax);
	};
}