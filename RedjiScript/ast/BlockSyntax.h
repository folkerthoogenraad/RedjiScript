#pragma once

#include "StatementSyntax.h"

#include <memory>
#include <vector>

namespace redji {

	class BlockSyntax : public StatementSyntax{
	public:
		std::vector<std::shared_ptr<StatementSyntax>> m_Statements;

		void accept(SyntaxVisitor &visitor) override { visitor.visit(*this); }
		virtual void toString(std::ostream &stream) const;
	};

}