#include "SymbolTable.h"

#include "ast/ClassSyntax.h"
#include "ast/FunctionSyntax.h"

#include "logger/log.h"

#include <queue>

namespace redji {
	SymbolTable::SymbolTable()
	{
		m_RootNamespace = std::make_shared<NamespaceSymbol>();
	}

	void SymbolTable::process(std::shared_ptr<CompilationUnit> unit)
	{
		// All things are processed in two steps:
		// Preprocessing :	Adding the name to the enclosing namespace etc (outlining)
		// Processing :		Adding the actual contents (processing)

		m_RootNamespace = std::make_shared<NamespaceSymbol>();
		m_RootNamespace->m_Name = "";

		std::queue<std::shared_ptr<ClassSymbol>> classQueue;

		// First look at all the classes
		// Register the available classes
		for (auto stm : unit->m_Statements) {
			auto cls = std::dynamic_pointer_cast<ClassSyntax>(stm);

			if (cls == nullptr)
				continue;

			auto type = std::make_shared<ClassSymbol>();

			type->m_Name = cls->m_Name.m_Name;
			type->m_Definition = cls;

			classQueue.push(type);

			m_RootNamespace->addClass(type);
		}

		// Set the symbol for members of classQueue
		while (!classQueue.empty()) {
			auto symbol = classQueue.front();
			classQueue.pop();

			auto cls = symbol->m_Definition;

			for (auto m : cls->m_Members) {
				auto member = std::make_shared<MemberSymbol>();

				auto type = std::make_shared<TypeSymbol>();
				type->m_Class = m_RootNamespace->findClassByName(m->m_Type->m_Name);

				member->m_Name = m->m_Name;
				member->setType(type);

				symbol->addMember(member);
			}
		}

		std::queue<std::shared_ptr<FunctionSymbol>> functionsQueue;

		// Look at functions next
		for (auto stm : unit->m_Statements) {
			auto fn = std::dynamic_pointer_cast<FunctionSyntax>(stm);

			if (fn == nullptr)
				continue;

			auto function = std::make_shared<FunctionSymbol>();

			// Get the name
			function->m_Name = fn->m_Name.m_Name;

			// Get the return symbol
			function->m_ReturnType = createTypeSyntax(fn->m_ReturnType);

			// Get the parameters
			for (auto& p : fn->m_Parameters) {
				auto parameter = std::make_shared<ParameterSymbol>();

				parameter->m_Name = p.m_Name;
				parameter->m_Type = createTypeSyntax(p.m_Type);

				function->m_Parameters.push_back(parameter);
			}

			functionsQueue.push(function);

			m_RootNamespace->addFunction(function);
		}
	
		while (!functionsQueue.empty()) {
			auto symbol = functionsQueue.front();
			functionsQueue.pop();

		}

	}

	std::shared_ptr<TypeSymbol> SymbolTable::createTypeSyntax(std::shared_ptr<TypeSyntax> syntax)
	{
		std::shared_ptr<IClassSymbol> cls;

		if (syntax != nullptr) {
			cls = m_RootNamespace->findClassByName(syntax->m_Name);
		}
		else {
			cls = BuiltinClassSymbol::getVoid();
		}

		auto type = std::make_shared<TypeSymbol>();
		type->m_Definition = syntax;
		type->m_Class = cls;
		

		return type;
	}
}
