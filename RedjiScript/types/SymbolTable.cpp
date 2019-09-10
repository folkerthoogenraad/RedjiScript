#include "SymbolTable.h"

#include "ast/ClassSyntax.h"
#include "ast/FunctionSyntax.h"

#include <queue>

namespace redji {
	SymbolTable::SymbolTable()
	{
		m_RootNamespace = std::make_shared<NamespaceSymbol>();

		logger.setPrefix("TypeChecker");
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

			insertSymbol(cls.get(), type);
			m_RootNamespace->addClass(type);
		}

		// Set the symbol for members of classQueue
		while (!classQueue.empty()) {
			auto symbol = classQueue.front();
			classQueue.pop();

			auto cls = symbol->m_Definition;

			for (auto m : cls->m_Members) {
				auto member = std::make_shared<MemberSymbol>();

				auto type = createTypeSymbol(m->m_Type);

				if (type == nullptr) {
					logger.logError("Unkown type of member : ", *m);
				}

				member->m_Name = m->m_Name;
				member->setType(type);

				insertSymbol(m.get(), member);
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
			function->m_ReturnType = createTypeSymbol(fn->m_ReturnType);
			function->m_Definition = fn;

			insertSymbol(fn.get(), function);

			// Get the parameters
			for (auto& p : fn->m_Parameters) {
				auto parameter = std::make_shared<ParameterSymbol>();

				parameter->m_Name = p->m_Name;
				parameter->m_Type = createTypeSymbol(p->m_Type);

				function->m_Parameters.push_back(parameter);

				insertSymbol(p.get(), parameter);
			}

			functionsQueue.push(function);

			m_RootNamespace->addFunction(function);
		}
	
		while (!functionsQueue.empty()) {
			auto symbol = functionsQueue.front();
			functionsQueue.pop();

			processFunction(symbol);
		}

	}

	void SymbolTable::insertSymbol(SyntaxNode* node, std::shared_ptr<Symbol> sym)
	{
		m_Symbols[node] = sym;
	}

	std::shared_ptr<Symbol> SymbolTable::findSymbol(SyntaxNode* node)
	{
		auto f = m_Symbols.find(node);
		if (f == m_Symbols.end()) {
			return nullptr;
		}
		return f->second;
	}

	std::shared_ptr<TypeSymbol> SymbolTable::createTypeSymbol(std::shared_ptr<TypeSyntax> syntax)
	{
		std::shared_ptr<IClassSymbol> cls;

		if (syntax != nullptr) {
			if (syntax->m_Name == "int")
				cls = BuiltinClassSymbol::getInteger();
			else if (syntax->m_Name == "float")
				cls = BuiltinClassSymbol::getFloat();
			else if (syntax->m_Name == "string")
				cls = BuiltinClassSymbol::getString();
			else
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

	void SymbolTable::processFunction(std::shared_ptr<FunctionSymbol> func)
	{
		func->m_RootScope = std::make_shared<SymbolScope>();
		
		for (auto &i : func->m_Parameters) {
			func->m_RootScope->addDefinition(i);
		}

		processStatement(func->m_RootScope, func->m_Definition->m_Body);
	}

	void SymbolTable::processStatement(std::shared_ptr<SymbolScope> scope, std::shared_ptr<StatementSyntax> statement)
	{
		// Process blocks
		if (auto block = std::dynamic_pointer_cast<BlockSyntax>(statement)) {
			// Create a child scope
			auto scp = scope->createChildScope(block);

			// Process all the statements in that block
			for (auto &i : block->m_Statements) {
				processStatement(scp, i);
			}
		}

		// Process the expression
		if (auto expressionStatement = std::dynamic_pointer_cast<ExpressionStatementSyntax>(statement)) {
			processExpression(scope, expressionStatement->m_Expression);
		}

		// Process locals
		if (auto local = std::dynamic_pointer_cast<LocalSyntax>(statement)) {
			auto localSymbol = std::make_shared<LocalSymbol>();
			localSymbol->m_Name = local->m_Name;
			localSymbol->m_Definition = local;

			if (local->m_InitialValue != nullptr) {
				processExpression(scope, local->m_InitialValue);
			}

			if (local->m_Type != nullptr) {
				localSymbol->m_Type = createTypeSymbol(local->m_Type);
			}
			else {
				localSymbol->m_Type = scope->findTypeOf(local->m_InitialValue);
			}

			if (localSymbol->m_Type == nullptr) {
				logger.logError("Unknown type for variable ", localSymbol->m_Name);
			}

			if (local->m_InitialValue != nullptr) {
				if (!localSymbol->m_Type->equals(*scope->findTypeOf(local->m_InitialValue))) {
					logger.logError("Type mismatch at variable ", localSymbol->m_Name, ".");
				}
			}

			insertSymbol(local.get(), localSymbol);
			scope->addDefinition(localSymbol);
		}
	}

	void SymbolTable::processExpression(std::shared_ptr<SymbolScope> scope, std::shared_ptr<ExpressionSyntax> expression)
	{
		// Literals
		if (auto literalSyntax = std::dynamic_pointer_cast<LiteralSyntax>(expression)) {
			auto type = std::make_shared<TypeSymbol>();
			type->m_Class = BuiltinClassSymbol::getVoid();

			switch (literalSyntax->m_Type) {
			case LiteralSyntax::Integer:
				type->m_Class = BuiltinClassSymbol::getInteger();
				break;
			case LiteralSyntax::Float:
				type->m_Class = BuiltinClassSymbol::getFloat();
				break;
			case LiteralSyntax::String:
				type->m_Class = BuiltinClassSymbol::getString();
				break;
			default:
				logger.logUnimplemented("Unimplemented literal type : ", literalSyntax->m_Type, " (", *literalSyntax, ")");
				break;
			}

			scope->setTypeOf(literalSyntax, type);
		}

		// Operators
		else if (auto operatorSyntax = std::dynamic_pointer_cast<OperatorSyntax>(expression)) {
			// TODO operator overloading, etc
			// Currently it only uses the type of the first thing in the operators (doesn't really check)
			processExpression(scope, operatorSyntax->m_Lhs);
			processExpression(scope, operatorSyntax->m_Rhs);

			scope->setTypeOf(operatorSyntax, scope->findTypeOf(operatorSyntax->m_Lhs));
		}

		// Identifier
		else if (auto identifierSyntax = std::dynamic_pointer_cast<IdentifierSyntax>(expression)) {
			auto var = scope->findDefinition(identifierSyntax->m_Name);

			if (var == nullptr) {
				logger.logError("Unkown type of ", *identifierSyntax, ".");
				return;
			}

			scope->setTypeOf(identifierSyntax, var->m_Type);
		}

		// Lookup
		else if (auto lookupSyntax = std::dynamic_pointer_cast<LookupSyntax>(expression)) {
			processExpression(scope, lookupSyntax->m_Lhs);

			auto type = scope->findTypeOf(lookupSyntax->m_Lhs);

			if (type == nullptr) {
				logger.logError("Unknown class for ", *lookupSyntax);
				return;
			}

			auto cls = std::dynamic_pointer_cast<ClassSymbol>(type->m_Class);

			if (cls == nullptr) {
				logger.logError("Type is not a class :", *lookupSyntax);
				return;
			}

			auto v = cls->findVariableByName(lookupSyntax->m_Name);

			if (v == nullptr) {
				logger.logError("Class ", cls->m_Name, " has no member named ", lookupSyntax->m_Name);
				return;
			}

			scope->setTypeOf(lookupSyntax, v->m_Type);
		}

		// New
		else if (auto newSyntax = std::dynamic_pointer_cast<NewSyntax>(expression)) {
			auto type = createTypeSymbol(newSyntax->m_Type);

			scope->setTypeOf(newSyntax, type);
		}

		// Invokation
		else if (auto invokeSyntax = std::dynamic_pointer_cast<InvokeSyntax>(expression)) {

			for (auto &arg : invokeSyntax->m_Arguments) {
				processExpression(scope, arg);
			}

			// Finding the target function
			std::shared_ptr<FunctionSymbol> targetFunction = nullptr;

			// If its directly a name
			if (auto name = std::dynamic_pointer_cast<IdentifierSyntax>(invokeSyntax->m_Lhs)) {
				auto functions = m_RootNamespace->findFunctionsByName(name->m_Name);

				for (auto &fn : functions) {
					// Check if the count matches
					if (fn->m_Parameters.size() != invokeSyntax->m_Arguments.size())
						continue;

					bool match = true;

					// Check if the types match
					for (int i = 0; i < fn->m_Parameters.size(); i++) {
						auto pType = fn->m_Parameters[i]->m_Type;
						auto aType = scope->findTypeOf(invokeSyntax->m_Arguments[i]);

						if (pType == nullptr || aType == nullptr) {
							match = false;
							break;
						}

						if(!pType->equals(*aType)){
							match = false;
							break;
						}
					}

					if (match) {
						targetFunction = fn;
						break;
					}
				}
			}

			// Else break for now
			else {
				logger.logUnimplemented("Only name invocation allowed (", *invokeSyntax, ")");
			}
		

			// Using the target function
			if (targetFunction != nullptr) {
				scope->setTypeOf(invokeSyntax, targetFunction->m_ReturnType);
			}
			else {
				logger.logError("Unable to find function ", *invokeSyntax);
			}
		}

		// Unimplemented
		else {
			logger.logUnimplemented("Unprocessed expression: ", *expression);
		}


	}
}
