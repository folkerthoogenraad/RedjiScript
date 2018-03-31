#include "Parser.h"

#include "logger/log.h"


#include <sstream>
#include <assert.h>

namespace redji {

	Parser::Parser(std::vector<Token> tokens)
		: m_Tokens(std::move(tokens))
	{ }

	const Token & Parser::current()
	{
		return m_Tokens[m_Index];
	}

	const Token & Parser::next()
	{
		m_Index++;
		if (m_Index >= m_Tokens.size()) {
			m_Index = m_Tokens.size() - 1;
		}

		return current();
	}

	std::shared_ptr<BlockSyntax> Parser::parseAll()
	{
		std::shared_ptr<BlockSyntax> block = std::make_shared<BlockSyntax>();

		while (current()) {
			auto statement = parseStatement();

			if (statement == nullptr)
				break;

			block->m_Statements.push_back(statement);
		}

		return block;
	}

	std::shared_ptr<StatementSyntax> Parser::parseStatement()
	{
		//First, lets find all the modifiers
		std::vector<Token> modifiers;

		while (isModifier(current())) {
			modifiers.push_back(current());
			next();
		}

		//If its a keyword, see what we can do
		if (isKeyword(current())) {
			if (current().m_Type == Token::KeywordFunction) {
				return parseFunction(modifiers);
			}
			else {
				unexpectedToken();
				next();
				return nullptr;
			}
		}
		else if (modifiers.size() > 0) {
			LOG_WARNING("Warning! Illigal modifiers. " << current()); // TODO replace all LOG_WARNING and LOG_ERROR with an error list
		}

		//If its a block
		if (current().m_Type == Token::OpenCurly) {
			return parseBlock();
		}
		else if (current().m_Type == Token::LineEnd) {
			next();
			return parseStatement();
		}

		//Else, it basically always is an expression
		else {
			if (auto e = parseExpression()) {
				auto statement = std::make_shared<ExpressionStatementSyntax>();
				statement->m_Expression = e;
				return statement;
			}
			else {
				unexpectedToken();
				return nullptr;
			}
		}

		assert(false);
		return nullptr;
	}

	std::shared_ptr<ExpressionSyntax> Parser::parseExpression()
	{
		// Well this is the interesting bit

		return nullptr;
	}

	std::shared_ptr<BlockSyntax> Parser::parseBlock()
	{
		if (current().m_Type != Token::OpenCurly) {
			unexpectedToken(current(), Token::OpenCurly);
			return nullptr;
		}

		// Consume the OpenCurly
		next();

		std::shared_ptr<BlockSyntax> block = std::make_shared<BlockSyntax>();

		while (current().m_Type != Token::CloseCurly) {
			auto statement = parseStatement();

			if (statement == nullptr)
				break;

			block->m_Statements.push_back(statement);
		}

		if (current().m_Type != Token::CloseCurly) {
			unexpectedToken(current(), Token::CloseCurly);
			return block;
		}

		// Consume the close curly
		next();

		return block;
	}

	std::shared_ptr<FunctionSyntax> Parser::parseFunction(std::vector<Token> modifiers)
	{
		if (current().m_Type != Token::KeywordFunction) {
			unexpectedToken(current(), Token::KeywordFunction);
			return nullptr;
		}

		auto function = std::make_shared<FunctionSyntax>();

		if (next().m_Type != Token::Identifier) {
			unexpectedToken(current(), Token::Identifier);
			return nullptr;
		}

		// Parse the name
		function->m_Name = current().m_Data;

		if (next().m_Type != Token::OpenBracket) {
			unexpectedToken(current(), Token::OpenBracket);
			return nullptr;
		}
		
		// Parse the parameters
		if (next().m_Type != Token::CloseBracket) {
			function->m_Parameters = parseNameAndTypeList();
		}

		if (current().m_Type != Token::CloseBracket) {
			unexpectedToken(current(), Token::CloseBracket);
			return nullptr;
		}

		// Consume the close bracket
		next();

		// Now we have "function <name> (<parameters>)" done

		// Is there a return type
		if (current().m_Type == Token::Colon || current().m_Type == Token::OperatorArrow) {
			next(); // Consume the thingy medingy ( -> or : )
			function->m_ReturnType = parseType();
		}

		// Now, finally, parse the body of the function
		function->m_Body = parseStatement();

		return function;
	}

	TypeSyntax Parser::parseType()
	{
		TypeSyntax type;

		// TODO create actual type parsing

		if (current().m_Type != Token::Identifier) {
			unexpectedToken(current(), Token::Identifier);
			return type;
		}

		type.m_Name = current().m_Data;

		// Consume the identifier
		next();

		return type;
	}

	NameAndTypeSyntax Parser::parseNameAndType()
	{
		NameAndTypeSyntax nameAndType;

		if (current().m_Type != Token::Identifier) {
			unexpectedToken(current(), Token::Identifier);
			return nameAndType;
		}

		nameAndType.m_Name = current().m_Data;
		
		if (next().m_Type != Token::Colon) {
			// Name and type without type.
			return nameAndType;
		}

		next();

		nameAndType.m_Type = parseType();

		return nameAndType;
	}

	std::vector<NameAndTypeSyntax> Parser::parseNameAndTypeList()
	{
		std::vector<NameAndTypeSyntax> list;

		// Add the first item
		list.push_back(parseNameAndType());

		while (current().m_Type == Token::Seperator) {
			next();
			list.push_back(parseNameAndType());
		}

		return std::move(list);
	}

	void Parser::unexpectedToken(Token token, std::vector<Token::Type> expected)
	{
		if (expected.size() == 0) {
			LOG("Unexpected token " << token << ".");
		}
		else if (expected.size() == 1) {
			LOG("Unexpected token " << token << ". Expected : " << expected[0]);
		}
		else {
			std::stringstream stream;

			stream << "Unexpected token " << token << ". Expected : ";
			
			for (int i = 0; i < expected.size(); i++) {
				stream << expected[i];

				if (i == expected.size() - 2) {
					stream << " or ";
				}
				else if (i != expected.size() - 1) {
					stream << ", ";
				}
			}
			stream << ".";

			LOG(stream.str());
		}
	}

}
