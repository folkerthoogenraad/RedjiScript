#pragma once

namespace redji {

	enum class SymbolType {
		Function,
		Class,

		Module,

		Parameter,
		Local,
		Variable
	};

	struct Symbol {
		Symbol* parent;

		virtual SymbolType getType() = 0;
	};

	struct FunctionSymbol : public Symbol {
		virtual SymbolType getType() { return SymbolType::Function; };
	};

	struct ClassSymbol : public Symbol {
		std::string name;

		virtual SymbolType getType() { return SymbolType::Class; };
	};

	struct VariableSymbol : public Symbol {
		virtual SymbolType getType() { return SymbolType::Variable; };
	};

	struct ModuleSymbol : public Symbol {
		std::string name;

		std::vector<std::shared_ptr<ClassSymbol>> classes;
		std::vector<std::shared_ptr<FunctionSymbol>> functions;

		virtual SymbolType getType() { return SymbolType::Module; };
	};
}