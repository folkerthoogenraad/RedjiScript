#include "Language.h"
#include <cctype>

namespace redji {

	bool isWhitespace(char c)
	{
		return std::isspace(c);
	}

	bool isCharacter(char c)
	{
		return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
	}

	bool isNumber(char c)
	{
		return c >= '0' && c <= '9';
	}
}
