#include "Symbol.h"

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
	std::shared_ptr<BuiltinClassSymbol> BuiltinClassSymbol::getInteger()
	{
		static auto type = std::make_shared<BuiltinClassSymbol>(Integer);

		return type;
	}
}
