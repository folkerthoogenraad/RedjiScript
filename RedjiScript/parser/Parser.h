#pragma once

#include <vector>
#include <memory>


#include "token/Token.h"

#include "ast/StatementSyntax.h"
#include "ast/FunctionSyntax.h"
#include "ast/NameAndTypeSyntax.h"
#include "ast/TypeSyntax.h"
#include "ast/BlockSyntax.h"
#include "ast/VariableSyntax.h"
#include "ast/TypeNameSyntax.h"
#include "ast/ExpressionStatementSyntax.h"
#include "ast/ReturnSyntax.h"
#include "ast/ClassSyntax.h"

namespace redji {

	
	class Parser {
		size_t m_Index = 0;
		std::vector<Token> m_Tokens;
	public:
		Parser(std::vector<Token> tokens);

		const Token &current();
		const Token &next();
		
		std::shared_ptr<BlockSyntax> parseAll();
		std::shared_ptr<StatementSyntax> parseStatement();
	protected:
		std::shared_ptr<ExpressionSyntax> parseExpression(int detail = 7);
		std::shared_ptr<ExpressionSyntax> parseExpressionSimple();

		std::shared_ptr<BlockSyntax> parseBlock();
		std::shared_ptr<ClassSyntax> parseClass(std::vector<Token> modifiers);
		std::shared_ptr<FunctionSyntax> parseFunction(std::vector<Token> modifiers);
		std::shared_ptr<VariableSyntax> parseVariable(std::vector<Token> modifiers);

		TypeSyntax parseType();
		TypeNameSyntax parseTypeName();
		NameAndTypeSyntax parseNameAndType();
		std::vector<NameAndTypeSyntax> parseNameAndTypeList();
		
		void unexpectedToken() { unexpectedToken(current(), std::vector<Token::Type>()); }
		void unexpectedToken(Token token) { unexpectedToken(token, std::vector<Token::Type>()); };
		void unexpectedToken(Token token, Token::Type expected) { unexpectedToken(token, std::vector<Token::Type>{ expected }); };
		void unexpectedToken(Token token, std::vector<Token::Type> expected);
	};
}