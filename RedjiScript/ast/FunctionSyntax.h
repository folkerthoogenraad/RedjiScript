#pragma once

#include <optional>
#include <string>

#include "StatementSyntax.h"
#include "NameAndTypeSyntax.h"
#include "TypeSyntax.h"

namespace redji {

	class FunctionSyntax : public StatementSyntax {
	public:
		std::string m_Name;
		std::optional<TypeSyntax> m_ReturnType;

		std::vector<NameAndTypeSyntax> m_Parameters;

		std::shared_ptr<StatementSyntax> m_Body;
	};

}