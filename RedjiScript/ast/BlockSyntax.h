#pragma once

#include "StatementSyntax.h"

#include <memory>
#include <vector>

namespace redji {

	class BlockSyntax : public StatementSyntax{
	public:
		std::vector<std::shared_ptr<StatementSyntax>> m_Statements;
	};

}