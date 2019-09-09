#pragma once

#include <vector>
#include <memory>
#include <map>

#include "ast/AST.h"

namespace redji {

	struct IClassSymbol;
	struct ClassSymbol;
	struct NamedTypeSymbol;
	struct FunctionSymbol;

	struct Symbol {
		Symbol* m_Parent;

	public:
		virtual std::shared_ptr<ClassSymbol> findClassByName(const std::string &name);
		virtual std::shared_ptr<NamedTypeSymbol> findVariableByName(const std::string &name);
		virtual std::vector<std::shared_ptr<FunctionSymbol>> findFunctionsByName(const std::string &name);

		void setParent(Symbol *symbol);
	};

	struct TypeSymbol : public Symbol {
		std::shared_ptr<TypeSyntax> m_Definition;

		std::shared_ptr<IClassSymbol> m_Class;
		bool m_Array = false;
	public:
		bool equals(const TypeSymbol &other);
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

	struct LocalSymbol : public NamedTypeSymbol {
		std::shared_ptr<LocalSyntax> m_Definition;
	};

	struct IClassSymbol : public Symbol { };

	struct ClassSymbol : public IClassSymbol {
		std::string m_Name;

		std::vector<std::shared_ptr<MemberSymbol>> m_Members;

		std::shared_ptr<ClassSyntax> m_Definition;
	public:
		void setName(std::string name);

		void addMember(std::shared_ptr<MemberSymbol> mem);
		virtual std::shared_ptr<NamedTypeSymbol> findVariableByName(const std::string &name);
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

		std::shared_ptr<ClassSymbol> findClassByName(const std::string &name) override;
		std::vector<std::shared_ptr<FunctionSymbol>> findFunctionsByName(const std::string &name) override;
	};

	struct SymbolScope {
		SymbolScope *m_Parent;
		std::map<std::shared_ptr<ExpressionSyntax>, std::shared_ptr<TypeSymbol>> m_Expressions;

		std::vector<std::shared_ptr<SymbolScope>> m_Children;
		std::vector<std::shared_ptr<NamedTypeSymbol>> m_Variables;
	public:
		void setTypeOf(std::shared_ptr<ExpressionSyntax> syntax, std::shared_ptr<TypeSymbol> type);
		std::shared_ptr<TypeSymbol> findTypeOf(std::shared_ptr<ExpressionSyntax> syntax);

		void setParent(SymbolScope *scope);

		std::shared_ptr<SymbolScope> createChildScope();

		std::shared_ptr<NamedTypeSymbol> findDefinition(const std::string &name);
		void addDefinition(std::shared_ptr<NamedTypeSymbol> symbol);
	};


	struct FunctionSymbol : public Symbol {
		std::string m_Name;
		std::shared_ptr<TypeSymbol> m_ReturnType;
		std::vector<std::shared_ptr<ParameterSymbol>> m_Parameters;

		std::shared_ptr<FunctionSyntax> m_Definition;
		std::shared_ptr<SymbolScope> m_RootScope;
	public:
		void setName(std::string name);

		void setReturnType(std::shared_ptr<TypeSymbol> sym);
		void addParameter(std::shared_ptr<ParameterSymbol> mem);
	};
}