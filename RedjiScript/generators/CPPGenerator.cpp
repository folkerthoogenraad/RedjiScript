#include "CPPGenerator.h"

#include "ast/AST.h"

#include <assert.h>

namespace redji {
	std::string redji::CPPGenerator::generate(std::shared_ptr<SymbolTable> m_Symbols, CompilationUnit & node)
	{
		this->m_Symbols = m_Symbols;
		stream.clear();

		print("#include <string>\n");
		print("#include <memory>\n");

		node.accept(*this);

		return stream.str();
	}

	void CPPGenerator::visit(SyntaxNode & node)
	{
		print("ERROR");
	}


	void CPPGenerator::visit(CompilationUnit & node)
	{
		for (auto &i : node.m_Statements)
			i->accept(*this);
	}

	void CPPGenerator::visit(ClassSyntax & node)
	{
		stream << "class ";
		node.m_Name.accept(*this);
		stream << " {\n";

		for (auto &mem : node.m_Members) {
			mem->accept(*this);
		}

		stream << "};\n";
	}

	void CPPGenerator::visit(FunctionSyntax & node)
	{
		m_FunctionSymbol = std::dynamic_pointer_cast<FunctionSymbol>(m_Symbols->findSymbol(&node));

		if (m_FunctionSymbol == nullptr) {
			logger.logError("No symbol available for function ", node);
			return;
		}

		printType(*m_FunctionSymbol->m_ReturnType);
		print(" ");
		print(m_FunctionSymbol->m_Name);
		print("(");
		
		for (int i = 0; i < node.m_Parameters.size(); i++) {
			printNode(*node.m_Parameters[i]);

			if (i != node.m_Parameters.size() - 1)
				print(", ");
		}

		print(")\n");

		if (!std::dynamic_pointer_cast<BlockSyntax>(node.m_Body)) {
			print("{\n");
		}

		m_SymbolScope = m_FunctionSymbol->m_RootScope;
		printNode(*node.m_Body);

		if (!std::dynamic_pointer_cast<BlockSyntax>(node.m_Body)) {
			print("}\n");
		}

		m_SymbolScope = nullptr;
		m_FunctionSymbol = nullptr;
	}

	void CPPGenerator::visit(MemberSyntax & node)
	{
		auto member = std::dynamic_pointer_cast<MemberSymbol>(m_Symbols->findSymbol(&node));

		if (member == nullptr) {
			logger.logError("No symbol available for member ", node);
			return;
		}

		printType(*member->m_Type);

		stream << " " << member->m_Name << ";\n";
	}

	void CPPGenerator::visit(ParameterSyntax & node)
	{
		auto parameter = std::dynamic_pointer_cast<ParameterSymbol>(m_Symbols->findSymbol(&node));

		if (parameter == nullptr) {
			logger.logError("No symbol available for member ", node);
			return;
		}

		printType(*parameter->m_Type);
		print(" ");
		print(parameter->m_Name);
	}

	void CPPGenerator::visit(TypeNameSyntax & node)
	{
		stream << node.m_Name;
	}

	void CPPGenerator::visit(TypeSyntax & node)
	{
	}

	void CPPGenerator::visit(LocalSyntax & node)
	{
		auto sym = std::dynamic_pointer_cast<LocalSymbol>(m_Symbols->findSymbol(&node));

		if (sym == nullptr) {
			logger.logError("No symbol for local ", node);
		}

		printType(*sym->m_Type);
		print(" ");
		print(sym->m_Name);

		// Print the initial value
		if (node.m_InitialValue != nullptr) {
			print(" = ");
			node.m_InitialValue->accept(*this);
		}

		print(";\n");
	}

	void CPPGenerator::visit(BlockSyntax & node)
	{
		auto s = m_SymbolScope;

		auto child = s->findChild(&node);

		if (child == nullptr) {
			logger.logError("No scope found for ", node);
		}

		// Set the symbol scope
		m_SymbolScope = child;

		print("{\n");
		// Do all the statements
		for (auto i : node.m_Statements) {
			i->accept(*this);
		}
		print("}\n");

		// Reset the symbol scope back
		m_SymbolScope = s;
	}

	void CPPGenerator::visit(StatmentSyntax & node)
	{
	}

	void CPPGenerator::visit(ExpressionStatementSyntax & node)
	{
		node.m_Expression->accept(*this);
		print(";\n");
	}

	void CPPGenerator::visit(ReturnSyntax & node)
	{
		print("return ");
		node.m_Body->accept(*this);
		print(";\n");
	}

	void CPPGenerator::visit(ExpressionSyntax & node)
	{
	}

	void CPPGenerator::visit(LookupSyntax & node)
	{
		node.m_Lhs->accept(*this);
		print(".");
		print(node.m_Name);
	}

	void CPPGenerator::visit(GenericInitializationSyntax & node)
	{
	}

	void CPPGenerator::visit(InvokeSyntax & node)
	{
		node.m_Lhs->accept(*this);
		print("(");

		for (int i = 0; i < node.m_Arguments.size(); i++) {
			node.m_Arguments[i]->accept(*this);

			if (i != node.m_Arguments.size() - 1)
				print(", ");
		}

		print(")");
	}

	void CPPGenerator::visit(PrefixOperatorSyntax & node)
	{
	}

	void CPPGenerator::visit(AssignmentSyntax & node)
	{
	}

	void CPPGenerator::visit(OperatorSyntax & node)
	{
		node.m_Lhs->accept(*this);

		switch (node.m_Operator) {
		case OperatorSyntax::Add: print("+"); break;
		case OperatorSyntax::Subtract: print("-"); break;
		case OperatorSyntax::Multiply: print("*"); break;
		case OperatorSyntax::Divide: print("/"); break;
		case OperatorSyntax::Modulo: print("%"); break;
		}
		node.m_Rhs->accept(*this);
	}

	void CPPGenerator::visit(ListSyntax & node)
	{
	}

	void CPPGenerator::visit(IdentifierSyntax & node)
	{
		print(node.m_Name);
	}

	void CPPGenerator::visit(LiteralSyntax & node)
	{
		if (node.m_Type == LiteralSyntax::String) {
			print("\"");
		}

		print(node.m_Data);

		if (node.m_Type == LiteralSyntax::String) {
			print("\"");
		}
	}

	void CPPGenerator::visit(BracketSyntax & node)
	{
	}

	void CPPGenerator::visit(NewSyntax & node)
	{
		print("std::make_shared<");

		// TODO print the actual type, not just the name given.
		print(node.m_Type->m_Name);

		// TODO make arguments and stuff
		print(">()");
	}

	void CPPGenerator::printNode(SyntaxNode & node)
	{
		node.accept(*this);
	}

	void CPPGenerator::printType(TypeSymbol & type)
	{
		auto cls = std::dynamic_pointer_cast<ClassSymbol>(type.m_Class);
		auto btn = std::dynamic_pointer_cast<BuiltinClassSymbol>(type.m_Class);

		if (cls != nullptr) {
			print("std::shared_ptr<");
			print(cls->m_Name);
			print(">");
		}
		else if (btn != nullptr) {
			switch (btn->m_Type) {
			case BuiltinClassSymbol::Integer: print("int"); break;
			case BuiltinClassSymbol::String: print("std::string"); break;
			case BuiltinClassSymbol::Float: print("float"); break;
			default: print("void"); break;
			}
		}
		else {
			logger.logError("Failed to write type.");
		}
		
		if (type.m_Array) {
			stream << "[]";
		}
	}
	void CPPGenerator::print(const std::string & string)
	{
		stream << string;
	}
}
