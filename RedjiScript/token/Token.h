#pragma once

#include <string>
#include <iostream>

namespace redji {

	struct Token {

		enum Type {
			Identifier,

			KeywordFunction,	//function
			KeywordVariable,	//variable
			KeywordStruct,		//struct
			KeywordClass,		//class

			ModifierPublic,	// public
			ModifierProtected,	// protected
			ModifierPrivate,	// private
			ModifierStatic,	// static
			ModifierLocal,		// local

			KeywordReturn,		// return
			KeywordIn,			// in

			KeywordIf,			// if
			KeywordElse,		// else
			KeywordFor,		// for
			KeywordWhile,		// while
			KeywordRepeat,		// repeat

			KeywordExtends,	// extends

			LiteralString,
			LiteralInteger,
			LiteralLong,
			LiteralFloat,
			LiteralDouble,

			LineEnd,			// ;
			Colon,				// :

			OpenBracket,		// (
			CloseBracket,		// )
			OpenCurly,			// {
			CloseCurly,			// }
			OpenSquare,			// [
			CloseSquare,		// ]

			//These should have their own thingy
			OperatorNew,		// new

			OperatorAdd,		// +
			OperatorSubtract,	// -
			OperatorDivide,	// /
			OperatorMultiply,	// *
			OperatorModulo,		// %

			OperatorBitShiftRight,	// >>
			OperatorBitShiftLeft,	// <<
			OperatorBitInverse,	// ~
			OperatorBitXOR,	// ^
			OperatorBitOR,	// |
			OperatorBitAnd,	// &

			OperatorLookup,	// .
			OperatorNot,		// !

			OperatorCompareGreater,	// >
			OperatorCompareLess,	// <
			OperatorCompareGreaterEQ,	// >=
			OperatorCompareLessEQ,	// <=
			OperatorCompareEqual,	// ==
			OperatorCompareNotEqual,	// !=

			OperatorLogicAnd,	// &&
			OperatorLogicOR,	// ||

			OperatorEquals,	// =
			OperatorAddEqual,	// +=
			OperatorSubtractEqual,	// -=
			OperatorMutliplyEqual,	// *=
			OperatorDivideEqual,	// /=
			OperatorModuloEqual,	// %=

			OperatorScope,		// ::

			Seperator,			// ,

			EndOfFile,				// FILE_EndOfFile
			Error,				// Error
			Unknown				// Unknown type
		};

		Type m_Type = Type::Unknown;
		std::string m_Data;

		int m_Line;
		int m_Character;

		friend std::ostream &operator<<(std::ostream &o, const Token &token);
		friend std::ostream &operator<<(std::ostream &o, const Type &token);

		friend bool isOperator(const Token &token);
		friend bool isValueType(const Token &token);

		friend bool isKeyword(const Token &token);
		friend bool isModifier(const Token &token);

		operator bool() const;
	};

	Token::Type getKeywordType(const std::string &name);
}