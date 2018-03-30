#pragma once

#include <vector>
#include <string>

#include "Characters.h"

#include "token/Token.h"

namespace redji {

	struct LexerError {
		int line;
		int characterIndex;
		char character;

		std::string toString() const;
	};

	class Lexer {
		Characters m_Chars;
		Token m_Current;
		std::vector<LexerError> m_Errors;
	public:
		Lexer(std::shared_ptr<std::istream> input);

		const Token &current();
		const Token &next();

		const std::vector<LexerError> getErrors() const { return m_Errors; }

		std::vector<Token> tokenize();

		operator bool();
	private:
		const void unexpectedCharacter();
	};


}