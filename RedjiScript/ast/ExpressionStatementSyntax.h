#pragma once

#include <memory>
#include "StatementSyntax.h"
#include "ExpressionSyntax.h"

namespace redji{

	class ExpressionStatementSyntax : public StatementSyntax {
	public:
		std::shared_ptr<ExpressionSyntax> m_Expression;
	};

}