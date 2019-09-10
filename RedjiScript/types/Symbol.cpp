#include "Symbol.h"

#include "logger/log.h"

namespace redji {
	void NamespaceSymbol::setName(std::string name)
	{
		m_Name = std::move(name);
	}

	void NamespaceSymbol::addClass(std::shared_ptr<ClassSymbol> cls)
	{
		cls->m_Parent = this;
		m_Classes.push_back(cls);
	}

	void NamespaceSymbol::addFunction(std::shared_ptr<FunctionSymbol> fn)
	{
		fn->m_Parent = this;
		m_Functions.push_back(fn);
	}

	std::shared_ptr<ClassSymbol> NamespaceSymbol::findClassByName(const std::string & name)
	{
		for (auto &i : m_Classes) {
			if (i->m_Name == name)
				return i;
		}
		return nullptr;
	}

	std::vector<std::shared_ptr<FunctionSymbol>> NamespaceSymbol::findFunctionsByName(const std::string & name)
	{
		std::vector<std::shared_ptr<FunctionSymbol>> functions;

		for (auto &i : m_Functions) {
			if (i->m_Name == name)
				functions.push_back(i);
		}

		return std::move(functions);
	}

	void ClassSymbol::setName(std::string name)
	{
		m_Name = name;
	}

	void ClassSymbol::addMember(std::shared_ptr<MemberSymbol> mem)
	{
		mem->m_Parent = this;
		m_Members.push_back(mem);
	}

	std::shared_ptr<NamedTypeSymbol> ClassSymbol::findVariableByName(const std::string & name)
	{
		for (int i = 0; i < m_Members.size(); i++) {
			if (m_Members[i]->m_Name == name)
				return m_Members[i];
		}

		return nullptr;
	}

	void FunctionSymbol::setName(std::string name)
	{
		m_Name = name;
	}
	void FunctionSymbol::setReturnType(std::shared_ptr<TypeSymbol> sym)
	{
		sym->m_Parent = this;
		m_ReturnType = sym;
	}
	void FunctionSymbol::addParameter(std::shared_ptr<ParameterSymbol> mem)
	{
		mem->m_Parent = this;
		m_Parameters.push_back(mem);
	}

	void NamedTypeSymbol::setType(std::shared_ptr<TypeSymbol> t)
	{
		t->m_Parent = this;
		m_Type = t;
	}


	std::shared_ptr<BuiltinClassSymbol> BuiltinClassSymbol::getFloat()
	{
		static auto type = std::make_shared<BuiltinClassSymbol>(Float);

		return type;
	}
	std::shared_ptr<BuiltinClassSymbol> BuiltinClassSymbol::getString()
	{
		static auto type = std::make_shared<BuiltinClassSymbol>(String);

		return type;
	}
	std::shared_ptr<BuiltinClassSymbol> BuiltinClassSymbol::getVoid()
	{
		static auto type = std::make_shared<BuiltinClassSymbol>(Void);

		return type;
	}
	std::string BuiltinClassSymbol::getName()
	{
		switch (m_Type) {
		case Integer: return "int";
		case Float: return "float";
		case String: return "string";
		case Void: return "void";
		}
	}
	std::shared_ptr<BuiltinClassSymbol> BuiltinClassSymbol::getInteger()
	{
		static auto type = std::make_shared<BuiltinClassSymbol>(Integer);

		return type;
	}
	std::shared_ptr<ClassSymbol> Symbol::findClassByName(const std::string & name)
	{
		return nullptr;
	}
	std::shared_ptr<NamedTypeSymbol> Symbol::findVariableByName(const std::string & name)
	{
		return nullptr;
	}
	std::vector<std::shared_ptr<FunctionSymbol>> Symbol::findFunctionsByName(const std::string & name)
	{
		return std::vector<std::shared_ptr<FunctionSymbol>>();
	}
	void Symbol::setParent(Symbol * symbol)
	{
		this->m_Parent = symbol;
	}

	void SymbolScope::setTypeOf(std::shared_ptr<ExpressionSyntax> syntax, std::shared_ptr<TypeSymbol> type)
	{
		if (m_Expressions.find(syntax) != m_Expressions.end()) {
			LOG_ERROR("Reinserting same expression again.");
		}
		m_Expressions[syntax] = type;
	}

	std::shared_ptr<TypeSymbol> SymbolScope::findTypeOf(std::shared_ptr<ExpressionSyntax> syntax)
	{
		if (m_Expressions.find(syntax) == m_Expressions.end()) {
			return nullptr;
		}

		return m_Expressions[syntax];
	}

	void SymbolScope::setParent(SymbolScope * scope)
	{
		m_Parent = scope;
	}
	std::shared_ptr<NamedTypeSymbol> SymbolScope::findDefinition(const std::string & name)
	{
		for (auto &i : m_Variables) {
			if (i->m_Name == name)
				return i;
		}

		if (m_Parent != nullptr)
			return m_Parent->findDefinition(name);

		return nullptr;
	}

	std::shared_ptr<SymbolScope> SymbolScope::findChild(BlockSyntax * block)
	{
		for (auto & c : m_Children) {
			if (c->m_Definition.get() == block)
				return c;
		}

		return nullptr;
	}

	void SymbolScope::addDefinition(std::shared_ptr<NamedTypeSymbol> symbol)
	{
		if (findDefinition(symbol->m_Name) != nullptr)
			LOG_ERROR("Duplicate definition of " << symbol->m_Name << ".");

		m_Variables.push_back(symbol);
	}

	std::shared_ptr<SymbolScope> SymbolScope::createChildScope(std::shared_ptr<BlockSyntax> block)
	{
		auto child = std::make_shared<SymbolScope>();
		child->m_Definition = block;
		child->setParent(this);

		m_Children.push_back(child);
		return child;
	}
	bool TypeSymbol::equals(const TypeSymbol & other)
	{
		return m_Class == other.m_Class && m_Array == other.m_Array;
	}
}
