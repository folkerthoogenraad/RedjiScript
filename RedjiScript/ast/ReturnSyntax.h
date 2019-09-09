#pragma once

#include <memory>

#include "StatementSyntax.h"
#include "ExpressionSyntax.h"

namespace redji {

	class ReturnSyntax : public StatementSyntax {
	public:
		std::shared_ptr<ExpressionSyntax> m_Body;

		void accept(SyntaxVisitor &visitor) override { visitor.visit(*this); }
		virtual void toString(std::ostream &stream) const;
	};

}