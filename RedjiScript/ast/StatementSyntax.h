#pragma once

#include "token/Token.h"
#include "SyntaxNode.h"

namespace redji {

	class StatementSyntax : public SyntaxNode{
	public:
		Token m_Token;

		virtual ~StatementSyntax() {};
	};

}