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

	std::shared_ptr<ExpressionSyntax> Parser::parseExpression(int detail)
	{
		// This obviously needs a lot more work and different shit, but whatever
		// This works for now

		//--------------------------------------------------------------//
		//Value types
		//--------------------------------------------------------------//
		if (detail <= 0) {
			return parseExpressionSimple();
		}
		//--------------------------------------------------------------//
		// ()
		//--------------------------------------------------------------//
		else if (detail == 1) {
			if (current().m_Type == Token::OpenBracket) {

				//Consume (
				next();

				//Parse a full expression!
				auto exp = parseExpression();

				if (current().m_Type != Token::CloseBracket) {
					unexpectedToken();

					// Why tho?
					return parseExpression(detail - 1);
				}

				//Consume )
				next();

				return exp;
			}
			else {
				return parseExpression(detail - 1);
			}
		}

		//--------------------------------------------------------------//
		//Lookups and invocations (TODO postfix operators like ++ and --)
		//--------------------------------------------------------------//
		else if (detail == 2) {
			auto lhs = parseExpression(detail - 1);

			while (current().m_Type == Token::OperatorLookup || current().m_Type == Token::OpenBracket) {

				if (current().m_Type == Token::OperatorLookup) {
					auto exp = std::make_shared<LookupSyntax>();
					exp->m_Token = current();

					next();
					
					auto rhs = parseExpression(detail - 1);

					exp->m_Lhs = lhs;
					exp->m_Rhs = rhs;

					lhs = exp;
				}
				else if (current().m_Type == Token::OpenBracket) {
					auto exp = std::make_shared<InvokeSyntax>();

					exp->m_Lhs = lhs;
					exp->m_Token = current();

					if (next().m_Type != Token::CloseBracket) {
						auto rhs = parseExpression(); // Parse a full expression between these brackets
						exp->m_Arguments = rhs;
						if (current().m_Type != Token::CloseBracket) {
							unexpectedToken();
						}
					}

					//Consume the closing bracket
					next();

					lhs = exp;
				}
			}

			return lhs;
		}

		//--------------------------------------------------------------//
		// Prefix operators
		//--------------------------------------------------------------//
		else if (detail == 3) {
			if (current().m_Type == Token::OperatorNot
				|| current().m_Type == Token::OperatorAdd
				|| current().m_Type == Token::OperatorSubtract) {
				auto exp = std::make_shared<PrefixOperatorSyntax>();

				exp->m_Token = current();

				switch(current().m_Type) {
				case Token::OperatorNot: exp->m_Operator = PrefixOperatorSyntax::Not; break;
				case Token::OperatorAdd: exp->m_Operator = PrefixOperatorSyntax::Plus; break;
				case Token::OperatorSubtract: exp->m_Operator = PrefixOperatorSyntax::Minus; break;
				default:
					// TODO 
					break;
				}

				// Consume the operator
				next();

				exp->m_Rhs = parseExpression(detail);

				return exp;
			}
			else {
				return parseExpression(detail - 1);
			}
		}

		//--------------------------------------------------------------//
		// Multiplication and division
		//--------------------------------------------------------------//
		else if (detail == 4) {
			auto lhs = parseExpression(detail - 1);

			while (current().m_Type == Token::OperatorMultiply
				|| current().m_Type == Token::OperatorDivide
				|| current().m_Type == Token::OperatorModulo) {

				auto exp = std::make_shared<OperatorSyntax>();
				exp->m_Token = current();

				exp->m_Lhs = lhs;

				if (current().m_Type == Token::OperatorMultiply)
					exp->m_Operator = OperatorSyntax::Multiply;
				
				else if (current().m_Type == Token::OperatorDivide)
					exp->m_Operator = OperatorSyntax::Divide;

				else if (current().m_Type == Token::OperatorModulo)
					exp->m_Operator = OperatorSyntax::Modulo;

				next(); // Consume the operator

				exp->m_Rhs = parseExpression(detail - 1);

				lhs = exp;
			}

			return lhs;
		}

		//--------------------------------------------------------------//
		// Adding and subtracting
		//--------------------------------------------------------------//
		else if (detail == 5) {
			auto lhs = parseExpression(detail - 1);

			while (current().m_Type == Token::OperatorAdd
				|| current().m_Type == Token::OperatorSubtract) {

				auto exp = std::make_shared<OperatorSyntax>();
				exp->m_Token = current();

				exp->m_Lhs = lhs;

				if(current().m_Type == Token::OperatorAdd)
					exp->m_Operator = OperatorSyntax::Add;
				else
					exp->m_Operator = OperatorSyntax::Subtract;
				
				next();
				exp->m_Rhs = parseExpression(detail - 1);

				lhs = exp;
			}

			return lhs;
		}

		//--------------------------------------------------------------//
		// Assignment
		//--------------------------------------------------------------//
		else if (detail == 6) {
			auto lhs = parseExpression(detail - 1);

			// TODO plus equals, etc etc
			if (current().m_Type == Token::OperatorEquals) {

				auto exp = std::make_shared<AssignmentSyntax>();
				exp->m_Token = current();

				exp->m_Lhs = lhs;

				next(); // Consume the operator
				
				exp->m_Rhs = parseExpression(detail);

				return exp;
			}

			return lhs;
		}

		//--------------------------------------------------------------//
		// Seperator
		//--------------------------------------------------------------//
		else if (detail == 7) {

			auto lhs = parseExpression(detail - 1);

			if (current().m_Type == Token::Seperator) {
				auto exp = std::make_shared<ListSyntax>();

				exp->m_List.push_back(lhs);

				while (current().m_Type == Token::Seperator) {
					next();

					auto e = parseExpression(detail - 1);

					exp->m_List.push_back(e);
				}

				return exp;
			}
			else {
				return lhs;
			}

		}

		else {
			return parseExpression(detail - 1);
		}

		//This should NEVER happen
		assert(false);

		return nullptr;
	}

	std::shared_ptr<ExpressionSyntax> Parser::parseExpressionSimple()
	{
		if (current().m_Type == Token::Identifier) {
			auto exp = std::make_shared<IdentifierSyntax>();
			exp->m_Token = current();
			exp->m_Name = current().m_Data;

			// Consume the name
			next();

			return exp;
		}
		else if (current().m_Type == Token::LiteralString) {
			auto exp = std::make_shared<LiteralSyntax>();
			exp->m_Token = current();
			exp->m_Data = current().m_Data;
			exp->m_Type = LiteralSyntax::String;
			next();
			return exp;
		}
		else if (current().m_Type == Token::LiteralInteger) {
			auto exp = std::make_shared<LiteralSyntax>();
			exp->m_Token = current();
			exp->m_Data = current().m_Data;
			exp->m_Type = LiteralSyntax::Integer;
			next();
			return exp;
		}
		else if (current().m_Type == Token::LiteralLong) {
			auto exp = std::make_shared<LiteralSyntax>();
			exp->m_Token = current();
			exp->m_Data = current().m_Data;
			exp->m_Type = LiteralSyntax::Long;
			next();
			return exp;
		}
		else if (current().m_Type == Token::LiteralDouble) {
			auto exp = std::make_shared<LiteralSyntax>();
			exp->m_Token = current();
			exp->m_Data = current().m_Data;
			exp->m_Type = LiteralSyntax::Double;
			next();
			return exp;
		}
		else if (current().m_Type == Token::LiteralFloat) {
			auto exp = std::make_shared<LiteralSyntax>();
			exp->m_Token = current();
			exp->m_Data = current().m_Data;
			exp->m_Type = LiteralSyntax::Float;
			next();
			return exp;
		}
		else {
			// TODO I should really figure out whether or not I should consume the token when its unexpected.
			// I probably should, because it can cause infinite loops if I don't.
			unexpectedToken();
			next();

			return nullptr;
		}
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
