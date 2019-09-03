#include "SymbolTable.h"

#include "ast/ClassSyntax.h"
#include "ast/FunctionSyntax.h"

#include "logger/log.h"

namespace redji {
	SymbolTable::SymbolTable(std::shared_ptr<BlockSyntax> generator)
	{
		createModule(generator);
	}

	void SymbolTable::insert(std::shared_ptr<SyntaxNode> node, std::shared_ptr<Symbol> symbol)
	{
		m_Symbols.insert(std::make_pair(node, symbol));
	}

	std::shared_ptr<ModuleSymbol> SymbolTable::createModule(std::shared_ptr<BlockSyntax> generator)
	{
		auto mod = std::make_shared<ModuleSymbol>();

		return mod;
	}
}
