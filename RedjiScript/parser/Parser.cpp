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

	std::shared_ptr<CompilationUnit> Parser::parseAll()
	{
		std::shared_ptr<CompilationUnit> block = std::make_shared<CompilationUnit>();

		while (current()) {
			auto statement = parseStatement();

			if (std::dynamic_pointer_cast<ExpressionStatementSyntax>(statement) != nullptr) {
				LOG("Can't add normal expressions in compilation unit");
				continue;
			}

			if (statement != nullptr)
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
			else if (current().m_Type == Token::KeywordVariable) {
				return parseLocal(modifiers);
			}
			else if (current().m_Type == Token::KeywordClass || current().m_Type == Token::KeywordStruct) {
				return parseStruct(modifiers);
			}
			else if (current().m_Type == Token::KeywordReturn) {
				auto returnStatement = std::make_shared<ReturnSyntax>();
				returnStatement->m_Token = current();
				
				next(); // Consume the return statement

				returnStatement->m_Body = parseExpression();

				return returnStatement;
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
		else if (current().m_Type == Token::LineEnd) { // This is actually quite ugly way to handle stuff. Probably should not be handled like this.
			next();
			// TODO create something like an "EmptyExpression" or something?
			return nullptr; // parseStatement();
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

				auto bracket = std::make_shared<BracketSyntax>();
				bracket->m_Expression = exp;

				return bracket;
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

			while (current().m_Type == Token::OperatorLookup || current().m_Type == Token::OpenBracket || current().m_Type == Token::OperatorCompareLess) {

				if (current().m_Type == Token::OperatorLookup) {
					auto exp = std::make_shared<LookupSyntax>();
					exp->m_Token = current();

					// Consume the . token
					next();
					
					if (current().m_Type != Token::Identifier) {
						unexpectedToken(current(), Token::Identifier);
						return nullptr;
					}
					
					exp->m_Name = current().m_Data;
					exp->m_Lhs = lhs;

					// Consume the identifier
					next();

					lhs = exp;
				}
				else if (current().m_Type == Token::OpenBracket) {
					auto exp = std::make_shared<InvokeSyntax>();

					exp->m_Lhs = lhs;
					exp->m_Token = current();

					next();

					// TODO check if this even works remotely
					while (current().m_Type != Token::CloseBracket) {
						auto rhs = parseExpression(6); // Parse a full expression between these brackets
						exp->m_Arguments.push_back(rhs);

						if (current().m_Type != Token::CloseBracket && current().m_Type != Token::Seperator) {
							unexpectedToken();
						}

						// If its a seperator, consume it
						if (current().m_Type == Token::Seperator) {
							next();
						}
					}

					//Consume the closing bracket
					next();

					lhs = exp;
				}
				else if (current().m_Type == Token::OperatorCompareLess) {
					auto exp = std::make_shared<GenericInitializeSyntax>();

					exp->m_Lhs = lhs;
					exp->m_Token = current();


					do {
						next();

						exp->m_GenericTypes.push_back(parseType());

					} while (current().m_Type == Token::Seperator);

					if (current().m_Type != Token::OperatorCompareGreater) {
						unexpectedToken();
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

			// Remove lineends
			while(current().m_Type == Token::LineEnd)
				next();
		}

		if (current().m_Type != Token::CloseCurly) {
			unexpectedToken(current(), Token::CloseCurly);
			return block;
		}

		// Consume the close curly
		next();

		return block;
	}

	std::shared_ptr<ClassSyntax> Parser::parseStruct(std::vector<Token> modifiers)
	{
		if (!(current().m_Type == Token::KeywordClass || current().m_Type == Token::KeywordStruct)) {
			unexpectedToken(current(), { Token::KeywordClass, Token::KeywordStruct} );
			return nullptr;
		}

		auto cls = std::make_shared<ClassSyntax>();

		cls->m_Type = ClassSyntax::Class;

		if(current().m_Type == Token::KeywordStruct)
			cls->m_Type = ClassSyntax::Struct;

		// Consume the keyword
		next();


		// Parse the correct name
		cls->m_Name = parseTypeName();

		if (current().m_Type != Token::OpenCurly) {
			unexpectedToken(current(), Token::OpenCurly);
			return nullptr;
		}

		// Consume the open curly
		next();

		while (current().m_Type != Token::CloseCurly) {
			// If end of file, break this loop
			if (current().m_Type == Token::EndOfFile) {
				unexpectedToken(current(), Token::CloseCurly);
				return nullptr;
			}

			// Ignore line ends
			if (current().m_Type == Token::LineEnd) {
				next();
				continue;
			}

			auto member = parseMember();

			if (member == nullptr) {
				unexpectedToken(current(), Token::Identifier);
				continue;
			}

			cls->m_Members.push_back(member);
		}

		// consume close curly
		next();

		// Return ti
		return cls;
	}

	std::shared_ptr<FunctionSyntax> Parser::parseFunction(std::vector<Token> modifiers)
	{
		if (current().m_Type != Token::KeywordFunction) {
			unexpectedToken(current(), Token::KeywordFunction);
			return nullptr;
		}

		std::shared_ptr<FunctionSyntax> function = std::make_shared<FunctionSyntax>();

		next();

		// Parse the name
		function->m_Name = parseTypeName();

		// Check if the function definition is even correctly formatted
		if (current().m_Type != Token::OpenBracket) {
			unexpectedToken(current(), Token::OpenBracket);
			return nullptr;
		}

		// Consume the (
		next();

		// Parse the parameters
		if (current().m_Type != Token::CloseBracket) {
			function->m_Parameters = parseParameterList();
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

	std::shared_ptr<LocalSyntax> Parser::parseLocal(std::vector<Token> modifiers)
	{
		if (current().m_Type != Token::KeywordVariable) {
			unexpectedToken(current(), Token::KeywordVariable);
			return nullptr;
		}

		auto variable = std::make_shared<LocalSyntax>();

		if (next().m_Type != Token::Identifier) {
			unexpectedToken(current(), Token::Identifier);
			return nullptr;
		}

		// Parse the name
		variable->m_Name = current().m_Data;

		next();

		if (current().m_Type == Token::Colon) {
			// Explicit type
			next(); // Consume the color
			variable->m_Type = parseType();
		}


		if (current().m_Type == Token::OperatorEquals) {
			// initial value
			next(); // Consume the operator
			variable->m_InitialValue = parseExpression(); // TODO detail level should quit be 7, because of multiple shit (var a,b,c = (3,1,5) );
		}

		return variable;
	}

	std::shared_ptr<MemberSyntax> Parser::parseMember()
	{
		auto ptr = std::make_shared<MemberSyntax>();

		// Make sure its an identifier
		if (current().m_Type != Token::Identifier) {	
			unexpectedToken(current(), Token::Identifier);
			return nullptr;
		}

		ptr->m_Name = current().m_Data;

		// Consuem the name
		next();
		
		// Check if :
		if (current().m_Type != Token::Colon) {
			unexpectedToken(current(), Token::Colon);
			return nullptr;
		}

		// Consume the :
		next();

		ptr->m_Type = parseType();
		return ptr;
	}

	std::shared_ptr<TypeSyntax> Parser::parseType()
	{
		auto type = std::make_shared<TypeSyntax>();

		// TODO create actual type parsing
		if (current().m_Type != Token::Identifier) {
			unexpectedToken(current(), Token::Identifier);
			return type;
		}

		type->m_Token = current();
		type->m_Name = current().m_Data;

		// Consume the identifier
		next();

		// The parsing of the generics
		if (current().m_Type == Token::OperatorCompareLess) {

			do {
				// Consume the seperator (or operatorcompareless) and check ID
				next();
				type->m_Generics.push_back(parseType());

			} while (current().m_Type == Token::Seperator);

			// Consume the seperator (or operatorcompareless) and check ID
			if (current().m_Type != Token::OperatorCompareGreater) {
				unexpectedToken(current(), Token::OperatorCompareGreater);
				// TODO continue anyway?
			}

			// Consume greater than operator
			next();
		}

		// Parse all the arrays
		while (current().m_Type == Token::OpenSquare){
			// Consume the OpenSquare
			next();

			TypeSyntax::Array array;

			array.type = TypeSyntax::Array::Heap;
			
			// If this is a stack array
			if (current().m_Type == Token::LiteralInteger) {
				array.type = TypeSyntax::Array::Stack;
				array.size = std::stoi(current().m_Data);
				next();
			}

			if (current().m_Type != Token::CloseSquare) {
				unexpectedToken(current(), Token::OperatorCompareGreater);
				// TODO continue anyway?
			}

			// Consume the square close
			next();

			// Add the parsed array to the type
			type->m_Arrays.push_back(array);
		}

		return type;
	}

	TypeNameSyntax Parser::parseTypeName()
	{
		TypeNameSyntax type;

		// First the name
		if (current().m_Type != Token::Identifier) {
			unexpectedToken(current(), Token::Identifier);
			return type;
		}

		// Set the name
		type.m_Token = current();
		type.m_Name = current().m_Data;

		next(); // Consume the ID

		// The parsing of the generic names
		if (current().m_Type == Token::OperatorCompareLess) {
			
			do {
				// Consume the seperator (or operatorcompareless) and check ID
				if (next().m_Type != Token::Identifier) {
					unexpectedToken(current(), Token::Identifier);
					return type;
				}

				type.m_GenericNames.push_back(current().m_Data);

				next(); // Consume the ID
			} while (current().m_Type == Token::Seperator);

			// Consume the seperator (or operatorcompareless) and check ID
			if (current().m_Type != Token::OperatorCompareGreater) {
				unexpectedToken(current(), Token::OperatorCompareGreater);
				// TODO continue anyway?
			}

			// Consume greater than operator
			next();
		}

		return type;
	}

	ParameterSyntax Parser::parseParameter()
	{
		ParameterSyntax nameAndType;

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

	std::vector<ParameterSyntax> Parser::parseParameterList()
	{
		std::vector<ParameterSyntax> list;

		// Add the first item
		list.push_back(parseParameter());

		while (current().m_Type == Token::Seperator) {
			next();
			list.push_back(parseParameter());
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
