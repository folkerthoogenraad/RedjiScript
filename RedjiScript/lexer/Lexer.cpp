#include "Lexer.h"
#include "Language.h"

#include <sstream>
#include <iostream>
#include <iomanip>      // std::setw
#include <vector>
#include "logger/log.h"

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

	Lexer::operator bool()
	{
		return m_Chars;
	}

	const void Lexer::unexpectedCharacter()
	{
		LOG_ERROR("Unexpected character at line " << m_Chars.line() << " (" << m_Chars.index() << ") : " << m_Chars.current());
	}

	std::ostream & operator<<(std::ostream & o, const Token & t)
	{
		return o << "(" << std::setw(16) << t.m_Type << " - " << std::setw(4) << t.m_Line << ":" << std::setw(3) << t.m_Character << ") "  << t.m_Data;
	}

	std::ostream & operator<<(std::ostream & o, const Token::Type & token)
	{
		switch (token) {
		case Token::Identifier:
			o << "Identifier";
			break;
		case Token::KeywordClass:
			o << "KeywordClass";
			break;
		case Token::KeywordStruct:
			o << "KeywordStruct";
			break;
		case Token::KeywordFunction:
			o << "KeywordFunction";
			break;
		case Token::KeywordVariable:
			o << "KeywordVariable";
			break;

		case Token::ModifierPublic:
			o << "ModifierPublic";
			break;
		case Token::ModifierProtected:
			o << "ModifierProtected";
			break;
		case Token::ModifierPrivate:
			o << "ModifierPrivate";
			break;
		case Token::ModifierStatic:
			o << "ModifierStatic";
			break;

		case Token::KeywordIn:
			o << "OPERATOR_KEYWORD";
			break;
		case Token::KeywordReturn:
			o << "KeywordReturn";
			break;

		case Token::KeywordIf:
			o << "OPERATOR_KEYWORD";
			break;
		case Token::KeywordElse:
			o << "KeywordElse";
			break;
		case Token::KeywordFor:
			o << "KeywordFor";
			break;
		case Token::KeywordWhile:
			o << "KeywordWhile";
			break;
		case Token::KeywordRepeat:
			o << "KeywordRepeat";
			break;
		case Token::KeywordExtends:
			o << "KeywordExtends";
			break;

		case Token::LiteralString:
			o << "LiteralString";
			break;
		case Token::LiteralInteger:
			o << "LiteralInteger";
			break;
		case Token::LiteralLong:
			o << "LiteralLong";
			break;
		case Token::LiteralFloat:
			o << "LiteralFloat";
			break;
		case Token::LiteralDouble:
			o << "LiteralDouble";
			break;

		case Token::LineEnd:
			o << "LineEnd";
			break;
		case Token::Colon:
			o << "Colon";
			break;

		case Token::OpenBracket:
			o << "OpenBracket";
			break;
		case Token::CloseBracket:
			o << "CloseBracket";
			break;
		case Token::OpenCurly:
			o << "OpenCurly";
			break;
		case Token::CloseCurly:
			o << "CloseCurly";
			break;
		case Token::OpenSquare:
			o << "OpenSquare";
			break;
		case Token::CloseSquare:
			o << "CloseSquare";
			break;

		case Token::OperatorNew:
			o << "OperatorNew";
			break;

		case Token::OperatorAdd:
			o << "OperatorAdd";
			break;
		case Token::OperatorSubtract:
			o << "OperatorSubtract";
			break;
		case Token::OperatorDivide:
			o << "OperatorDivide";
			break;
		case Token::OperatorMultiply:
			o << "OperatorMultiply";
			break;
		case Token::OperatorModulo:
			o << "OperatorModulo";
			break;
		case Token::OperatorLookup:
			o << "OperatorLookup";
			break;
		case Token::OperatorEquals:
			o << "OperatorEquals";
			break;
		case Token::OperatorScope:
			o << "OperatorScope";
			break;
		case Token::OperatorNot:
			o << "OperatorNot";
			break;
		case Token::OperatorCompareGreater:
			o << "OperatorCompareGreater";
			break;
		case Token::OperatorCompareLess:
			o << "OperatorCompareLess";
			break;
		case Token::OperatorCompareGreaterEQ:
			o << "OperatorCompareGreaterEQ";
			break;
		case Token::OperatorCompareLessEQ:
			o << "OperatorCompareLessEQ";
			break;
		case Token::OperatorCompareEqual:
			o << "OperatorCompareEqual";
			break;
		case Token::OperatorCompareNotEqual:
			o << "OperatorCompareNotEqual";
			break;
		case Token::OperatorAddEqual:
			o << "OperatorAddEqual";
			break;
		case Token::OperatorSubtractEqual:
			o << "OperatorSubtractEqual";
			break;
		case Token::OperatorMutliplyEqual:
			o << "OperatorMutliplyEqual";
			break;
		case Token::OperatorDivideEqual:
			o << "OperatorDivideEqual";
			break;
		case Token::OperatorModuloEqual:
			o << "OperatorModuloEqual";
			break;

		case Token::OperatorLogicAnd:
			o << "OperatorLogicAnd";
			break;
		case Token::OperatorLogicOR:
			o << "OperatorLogicOR";
			break;

		case Token::OperatorBitShiftRight:
			o << "OperatorBitShiftRight";
			break;
		case Token::OperatorBitShiftLeft:
			o << "OperatorBitShiftLeft";
			break;
		case Token::OperatorBitInverse:
			o << "OperatorBitInverse";
			break;
		case Token::OperatorBitXOR:
			o << "OperatorBitXOR";
			break;
		case Token::OperatorBitOR:
			o << "OperatorBitOR";
			break;
		case Token::OperatorBitAnd:
			o << "OperatorBitAnd";
			break;

		case Token::Seperator:
			o << "Seperator";
			break;
		case Token::EndOfFile:
			o << "EndOfFile";
			break;
		case Token::Error:
			o << "Error";
			break;
		case Token::Unknown:
			o << "Unknown";
			break;
		default:
			o << "Unimplmented";
			break;
		}
		return o;
	}

	const std::vector<Token::Type> valueTypes = {
		Token::Type::Identifier,

		Token::Type::LiteralString,
		Token::Type::LiteralInteger,
		Token::Type::LiteralLong,
		Token::Type::LiteralFloat,
		Token::Type::LiteralDouble,
	};

	const std::vector<std::pair<Token::Type, std::string>> keywordTypes = {
		{ Token::Type::KeywordClass, "class" },
	{ Token::Type::KeywordFunction, "function" },
	{ Token::Type::KeywordStruct, "struct" },
	{ Token::Type::KeywordVariable, "var" },

	{ Token::Type::KeywordFor, "for" },
	{ Token::Type::KeywordRepeat, "repeat" },
	{ Token::Type::KeywordWhile, "while" },
	{ Token::Type::KeywordIf, "if" },
	{ Token::Type::KeywordElse, "else" },

	{ Token::Type::KeywordIn, "in" },
	{ Token::Type::KeywordReturn, "return" },

	{ Token::Type::KeywordExtends, "extends" },
	};

	const std::vector<std::pair<Token::Type, std::string>> modifierTypes = {
		{ Token::Type::ModifierPublic, "public" },
	{ Token::Type::ModifierProtected, "protected" },
	{ Token::Type::ModifierPrivate, "private" },
	{ Token::Type::ModifierStatic, "static" },
	{ Token::Type::ModifierLocal, "local" },
	};

	const std::vector<std::pair<Token::Type, std::string>> keywordOperatorTypes = {
		{ Token::Type::OperatorNew, "new" },
	};

	const std::vector<Token::Type> operatorTypes = {
		Token::Type::OperatorAdd,		// +
		Token::Type::OperatorSubtract,	// -
		Token::Type::OperatorDivide,	// /
		Token::Type::OperatorMultiply,	// *
		Token::Type::OperatorModulo,		// %

		Token::Type::OperatorBitShiftRight,	// >>
		Token::Type::OperatorBitShiftLeft,	// <<
		Token::Type::OperatorBitInverse,	// ~
		Token::Type::OperatorBitXOR,	// ^
		Token::Type::OperatorBitOR,	// |
		Token::Type::OperatorBitAnd,	// &

		Token::Type::OperatorLookup,	// .
		Token::Type::OperatorNot,		// !

		Token::Type::OperatorCompareGreater,	// >
		Token::Type::OperatorCompareLess,	// <
		Token::Type::OperatorCompareGreaterEQ,	// >=
		Token::Type::OperatorCompareLessEQ,	// <=
		Token::Type::OperatorCompareEqual,	// ==
		Token::Type::OperatorCompareNotEqual,	// !=

		Token::Type::OperatorLogicAnd,	// &&
		Token::Type::OperatorLogicOR,	// ||

		Token::Type::OperatorEquals,	// =
		Token::Type::OperatorAddEqual,	// +=
		Token::Type::OperatorSubtractEqual,	// -=
		Token::Type::OperatorMutliplyEqual,	// *=
		Token::Type::OperatorDivideEqual,	// /=
		Token::Type::OperatorModuloEqual,	// %=

		Token::Type::OperatorScope,		// ::

		Token::Type::Seperator			// ,
	};

	bool isOperator(const Token & token)
	{
		auto t = token.m_Type;

		for (auto s : operatorTypes)
		{
			if (s == t)
				return true;
		}

		return false;
	}

	bool isValueType(const Token & token)
	{
		auto t = token.m_Type;

		for (auto s : valueTypes)
		{
			if (s == t)
				return true;
		}

		return false;
	}

	bool isKeyword(const Token & token)
	{
		auto t = token.m_Type;

		for (auto s : keywordTypes)
		{
			if (s.first == t)
				return true;
		}

		return false;
	}

	Token::Type getKeywordType(const std::string & name)
	{
		for (auto s : keywordTypes)
		{
			if (s.second == name)
				return s.first;
		}
		for (auto s : modifierTypes)
		{
			if (s.second == name)
				return s.first;
		}
		for (auto s : keywordOperatorTypes)
		{
			if (s.second == name)
				return s.first;
		}

		return Token::Error;
	}

	bool isModifier(const Token & token)
	{
		auto t = token.m_Type;

		for (auto s : modifierTypes)
		{
			if (s.first == t)
				return true;
		}

		return false;
	}

	Token::operator bool() const
	{
		return m_Type != EndOfFile && m_Type != Unknown;
	}
}