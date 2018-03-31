#include "Lexer.h"
#include "lang/Language.h"

#include <sstream>
#include <iostream>
#include <iomanip>      // std::setw
#include <vector>
#include "logger/log.h"

#include <assert.h>

#include <sstream>

#define LEXER_Error(x) throw std::exception(x)

namespace redji {

	Lexer::Lexer(std::shared_ptr<std::istream> input)
		:m_Chars(input)
	{
		//Prepare the first thing
		m_Chars.next();
	}

	const Token & Lexer::current()
	{
		return m_Current;
	}

	const Token & Lexer::next()
	{
		m_Current.m_Line = m_Chars.line();
		m_Current.m_Character = m_Chars.index();

		//End of stream
		if (!m_Chars.current()) {
			m_Current.m_Type = Token::EndOfFile;
			m_Current.m_Data = "EOF";
			return m_Current;
		}
		char c = m_Chars.current();

		//-------------------------------------------------------------------------------------//
		// Identifiers, keywords and operator keywords
		//-------------------------------------------------------------------------------------//
		if (isCharacter(c) || c == '_') {
			std::stringstream stream;

			if (c == '_') {
				LOG_WARNING("Warning! Names starting with '_' reserved for compiler names. Attempting to continue, but may result in unexpected behaviour!");
			}

			while ((isCharacter(c) || isNumber(c) || c == '_') && m_Chars) {
				stream << c;

				c = m_Chars.next();
			}

			m_Current.m_Data = stream.str();

			if (getKeywordType(m_Current.m_Data) != Token::Error)
				m_Current.m_Type = getKeywordType(m_Current.m_Data);
			else
				m_Current.m_Type = Token::Identifier;

			return m_Current;
		}

		//-------------------------------------------------------------------------------------//
		// Whitespace
		//-------------------------------------------------------------------------------------//
		else if (isWhitespace(c)) {
			//Consume the whitespace
			m_Chars.next();
			//Return the next token
			return next();
		}

		//-------------------------------------------------------------------------------------//
		// Numbers
		//-------------------------------------------------------------------------------------//

		else if (isNumber(c)) {

			std::stringstream stream;
			while (isNumber(c) && m_Chars) {
				stream << c;

				c = m_Chars.next();
			}

			//Dot number
			if (c == '.' && m_Chars) {
				stream << '.';
				c = m_Chars.next();
				while (isNumber(c) && m_Chars) {
					stream << c;

					c = m_Chars.next();
				}

				if (c == 'd' || isWhitespace(c) || !isCharacter(c)) {
					if (c == 'd')
						m_Chars.next();
					m_Current.m_Data = stream.str();
					m_Current.m_Type = Token::LiteralDouble;
					return m_Current;
				}
				else if (c == 'f') {
					m_Chars.next();
					m_Current.m_Data = stream.str();
					m_Current.m_Type = Token::LiteralFloat;
					return m_Current;
				}
				else {
					unexpectedCharacter();
				}
			}
			//Its a long
			else if (c == 'l') {
				m_Chars.next();

				m_Current.m_Data = stream.str();
				m_Current.m_Type = Token::LiteralLong;
				return m_Current;
			}
			else if (c == 'f') {
				m_Chars.next();

				m_Current.m_Data = stream.str();
				m_Current.m_Type = Token::LiteralFloat;
				return m_Current;
			}
			else if (c == 'd') {
				m_Chars.next();

				m_Current.m_Data = stream.str();
				m_Current.m_Type = Token::LiteralDouble;
				return m_Current;
			}
			else if (isCharacter(c)) {
				unexpectedCharacter();
			}
			else {
				m_Current.m_Data = stream.str();
				m_Current.m_Type = Token::LiteralInteger;
				return m_Current;
			}

			assert(false);
		}

		//-------------------------------------------------------------------------------------//
		// String literals
		//-------------------------------------------------------------------------------------//
		else if (c == '"') {
			std::stringstream stream;

			c = m_Chars.next();
			while (c != '"' && m_Chars) {
				stream << c;
				c = m_Chars.next();
			}

			//consume the last "
			m_Chars.next();

			m_Current.m_Data = stream.str();
			m_Current.m_Type = Token::LiteralString;

			return m_Current;
		}


		//-------------------------------------------------------------------------------------//
		// Brackets
		//-------------------------------------------------------------------------------------//
		else if (c == '{') {
			m_Current.m_Data = "{";
			m_Current.m_Type = Token::OpenCurly;
			m_Chars.next();
			return m_Current;
		}
		else if (c == '}') {
			m_Current.m_Data = "}";
			m_Current.m_Type = Token::CloseCurly;
			m_Chars.next();
			return m_Current;
		}
		else if (c == '(') {
			m_Current.m_Data = "(";
			m_Current.m_Type = Token::OpenBracket;
			m_Chars.next();
			return m_Current;
		}
		else if (c == ')') {
			m_Current.m_Data = ")";
			m_Current.m_Type = Token::CloseBracket;
			m_Chars.next();
			return m_Current;
		}
		else if (c == '[') {
			m_Current.m_Data = "[";
			m_Current.m_Type = Token::OpenSquare;
			m_Chars.next();
			return m_Current;
		}
		else if (c == ']') {
			m_Current.m_Data = "]";
			m_Current.m_Type = Token::CloseSquare;
			m_Chars.next();
			return m_Current;
		}

		//-------------------------------------------------------------------------------------//
		// Simple operators
		//-------------------------------------------------------------------------------------//
		else if (c == '.') {
			m_Current.m_Data = ".";
			m_Current.m_Type = Token::OperatorLookup;
			m_Chars.next();
			return m_Current;
		}
		else if (c == ',') {
			m_Current.m_Data = ",";
			m_Current.m_Type = Token::Seperator;
			m_Chars.next();
			return m_Current;
		}
		else if (c == ':') {
			c = m_Chars.next();
			if (c == ':') {
				m_Current.m_Data = "::";
				m_Current.m_Type = Token::OperatorScope;
				m_Chars.next();
			}
			else {
				m_Current.m_Data = ":";
				m_Current.m_Type = Token::Colon;
			}
			return m_Current;
		}
		else if (c == ';') {
			m_Current.m_Data = ";";
			m_Current.m_Type = Token::LineEnd;
			m_Chars.next();
			return m_Current;
		}
		//-------------------------------------------------------------------------------------//
		//Operators and assignment
		//-------------------------------------------------------------------------------------//
		else if (c == '=') {
			m_Current.m_Data = "=";
			m_Current.m_Type = Token::OperatorEquals;
			c = m_Chars.next();
			if (c == '=') {
				m_Chars.next();
				m_Current.m_Data = "==";
				m_Current.m_Type = Token::OperatorCompareEqual;
			}
			return m_Current;
		}
		else if (c == '+') {
			m_Current.m_Data = "+";
			m_Current.m_Type = Token::OperatorAdd;
			c = m_Chars.next();
			if (c == '=') {
				m_Chars.next();
				m_Current.m_Data = "+=";
				m_Current.m_Type = Token::OperatorAddEqual;
			}
			return m_Current;
		}
		else if (c == '-') {
			m_Current.m_Data = "-";
			m_Current.m_Type = Token::OperatorSubtract;
			c = m_Chars.next();
			if (c == '=') {
				m_Chars.next();
				m_Current.m_Data = "-=";
				m_Current.m_Type = Token::OperatorSubtractEqual;
			}
			else if (c == '>') {
				m_Chars.next();
				m_Current.m_Data = "->";
				m_Current.m_Type = Token::OperatorArrow;
			}
			return m_Current;
		}
		else if (c == '*') {
			m_Current.m_Data = "*";
			m_Current.m_Type = Token::OperatorMultiply;
			c = m_Chars.next();
			if (c == '=') {
				m_Chars.next();
				m_Current.m_Data = "*=";
				m_Current.m_Type = Token::OperatorMutliplyEqual;
			}
			return m_Current;
		}
		else if (c == '/') {
			m_Current.m_Data = "/";
			m_Current.m_Type = Token::OperatorDivide;
			c = m_Chars.next();
			if (c == '=') {
				m_Chars.next();
				m_Current.m_Data = "/=";
				m_Current.m_Type = Token::OperatorDivideEqual;
			}

			//One line comment
			else if (c == '/') {
				//Look for newline
				while (c != '\n' && m_Chars) {
					c = m_Chars.next();
				}
				return next();
			}

			//Mutli line comment
			else if (c == '*') {
				//Look for newline
				while (m_Chars) {
					c = m_Chars.next();
					if (c == '*') {
						c = m_Chars.next();
						if (c == '/') {
							m_Chars.next();
							break;
						}
					}
				}
				return next();
			}

			return m_Current;
		}
		else if (c == '%') {
			m_Current.m_Data = "%";
			m_Current.m_Type = Token::OperatorModulo;
			c = m_Chars.next();
			if (c == '=') {
				m_Chars.next();
				m_Current.m_Data = "%=";
				m_Current.m_Type = Token::OperatorModuloEqual;
			}
			return m_Current;
		}
		//-------------------------------------------------------------------------------------//
		//Bitwise and logic operators
		//-------------------------------------------------------------------------------------//
		else if (c == '~') {
			m_Current.m_Data = "~";
			m_Current.m_Type = Token::OperatorBitInverse;
			c = m_Chars.next();
			return m_Current;
		}
		else if (c == '^') {
			m_Current.m_Data = "^";
			m_Current.m_Type = Token::OperatorBitXOR;
			c = m_Chars.next();
			return m_Current;
		}
		else if (c == '|') {
			m_Current.m_Data = "|";
			m_Current.m_Type = Token::OperatorBitOR;
			c = m_Chars.next();
			if (c == '|') {
				m_Chars.next();
				m_Current.m_Data = "||";
				m_Current.m_Type = Token::OperatorLogicOR;
			}
			return m_Current;
		}
		else if (c == '&') {
			m_Current.m_Data = "&";
			m_Current.m_Type = Token::OperatorBitAnd;
			c = m_Chars.next();
			if (c == '&') {
				m_Chars.next();
				m_Current.m_Data = "&&";
				m_Current.m_Type = Token::OperatorLogicAnd;
			}
			return m_Current;
		}
		else if (c == '<') {
			m_Current.m_Data = "<";
			m_Current.m_Type = Token::OperatorCompareLess;
			c = m_Chars.next();
			if (c == '=') {
				m_Chars.next();
				m_Current.m_Data = "<=";
				m_Current.m_Type = Token::OperatorCompareLessEQ;
			}
			else if (c == '<') {
				m_Chars.next();
				m_Current.m_Data = "<<";
				m_Current.m_Type = Token::OperatorBitShiftLeft;
			}
			return m_Current;
		}
		else if (c == '>') {
			m_Current.m_Data = ">";
			m_Current.m_Type = Token::OperatorCompareGreater;
			c = m_Chars.next();
			if (c == '=') {
				m_Chars.next();
				m_Current.m_Data = ">=";
				m_Current.m_Type = Token::OperatorCompareGreaterEQ;
			}
			else if (c == '>') {
				m_Chars.next();
				m_Current.m_Data = ">>";
				m_Current.m_Type = Token::OperatorBitShiftRight;
			}
			return m_Current;
		}
		else if (c == '!') {
			m_Current.m_Data = "!";
			m_Current.m_Type = Token::OperatorNot;
			c = m_Chars.next();
			if (c == '=') {
				m_Chars.next();
				m_Current.m_Data = "!=";
				m_Current.m_Type = Token::OperatorCompareNotEqual;
			}
			return m_Current;
		}

		else {
			unexpectedCharacter();
			m_Chars.next();
			return next();
		}

		//Shouldn't be reached
		assert(false);

		return m_Current;
	}

	std::vector<Token> Lexer::tokenize()
	{
		std::vector<Token> tokens;

		while (next())
		{
			tokens.push_back(current());
		}

		// Add the end of file token
		tokens.push_back(current());

		return std::move(tokens);
	}

	Lexer::operator bool()
	{
		return m_Chars;
	}

	const void Lexer::unexpectedCharacter()
	{
		LexerError error;
		
		error.character = m_Chars.current();
		error.characterIndex = m_Chars.index();
		error.line = m_Chars.line();


		m_Errors.push_back(std::move(error));
		//LOG_ERROR("Unexpected character at line " << m_Chars.line() << " (" << m_Chars.index() << ") : " << m_Chars.current());
	}

	
	std::string LexerError::toString() const
	{
		std::stringstream stream;

		stream << "Unexpected character at line " << line << " (" << characterIndex << ") : " << character;

		return stream.str();
	}

}