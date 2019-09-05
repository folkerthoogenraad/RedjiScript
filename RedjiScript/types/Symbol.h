#pragma once

#include <vector>
#include <memory>

#include "ast/AST.h"

namespace redji {

	struct IClassSymbol;

	struct Symbol {
		Symbol* m_Parent;

	public:

	};

	struct TypeSymbol : public Symbol {
		std::shared_ptr<TypeSyntax> m_Definition;

		std::shared_ptr<IClassSymbol> m_Class;
		bool m_Array = false;
	};

	struct NamedTypeSymbol : public Symbol{
		std::string m_Name;
		std::shared_ptr<TypeSymbol> m_Type;

	public:
		void setType(std::shared_ptr<TypeSymbol> t);
	};

	struct ParameterSymbol : public NamedTypeSymbol {
		std::shared_ptr<FunctionSyntax> m_Definition;
	};

	struct MemberSymbol : public NamedTypeSymbol {
		std::shared_ptr<MemberSyntax> m_Definition;
	};

	struct FunctionSymbol : public Symbol {
		std::string m_Name;
		std::shared_ptr<TypeSymbol> m_ReturnType;
		std::vector<std::shared_ptr<ParameterSymbol>> m_Parameters;

		std::shared_ptr<FunctionSymbol> m_Definition;
	public:
		void setName(std::string name);

		void setReturnType(std::shared_ptr<TypeSymbol> sym);
		void addParameter(std::shared_ptr<ParameterSymbol> mem);

	};

	struct IClassSymbol : public Symbol { };

	struct ClassSymbol : public IClassSymbol {
		std::string m_Name;

		std::vector<std::shared_ptr<MemberSymbol>> m_Members;

		std::shared_ptr<ClassSyntax> m_Definition;
	public:
		void setName(std::string name);

		void addMember(std::shared_ptr<MemberSymbol> mem);
	};

	class BuiltinClassSymbol : public IClassSymbol {
	public:
		enum Subtype {
			Integer, Float, String, Void
		};

	private:
		Subtype m_Type;

	public:
		BuiltinClassSymbol(Subtype type) :m_Type(type) {}

		static std::shared_ptr<BuiltinClassSymbol> getFloat();
		static std::shared_ptr<BuiltinClassSymbol> getInteger();
		static std::shared_ptr<BuiltinClassSymbol> getString();
		static std::shared_ptr<BuiltinClassSymbol> getVoid();
	};



	struct NamespaceSymbol : public Symbol {
		std::string m_Name;

		std::vector<std::shared_ptr<ClassSymbol>> m_Classes;
		std::vector<std::shared_ptr<FunctionSymbol>> m_Functions;

	public:
		void setName(std::string name);

		void addClass(std::shared_ptr<ClassSymbol> cls);
		void addFunction(std::shared_ptr<FunctionSymbol> fn);

		std::shared_ptr<ClassSymbol> findClassByName(const std::string &name);
		std::vector<std::shared_ptr<FunctionSymbol>> findFunctionsByName(const std::string &name);
	};
}