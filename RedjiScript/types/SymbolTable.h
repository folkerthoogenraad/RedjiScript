#pragma once

#include <vector>
#include <map>
#include <memory>

#include "Symbol.h"

#include "ast/BlockSyntax.h"

namespace redji {

	class SymbolTable {
		std::map<std::shared_ptr<SyntaxNode>, std::shared_ptr<Symbol>> m_Symbols;

		SymbolTable *m_Parent;
		std::vector<std::shared_ptr<SymbolTable>> m_Children;
	public:
		SymbolTable(std::shared_ptr<BlockSyntax> generator);

		void insert(std::shared_ptr<SyntaxNode> node, std::shared_ptr<Symbol> symbol);
	private:
		std::shared_ptr<ModuleSymbol> createModule(std::shared_ptr<BlockSyntax> generator);
	};
}