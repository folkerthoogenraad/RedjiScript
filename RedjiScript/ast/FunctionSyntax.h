#pragma once

#include <optional>
#include <string>
#include <memory>

#include "StatementSyntax.h"
#include "NameAndTypeSyntax.h"
#include "TypeSyntax.h"
#include "TypeNameSyntax.h"

namespace redji {

	class FunctionSyntax : public StatementSyntax {
	public:
		TypeNameSyntax m_Name;
		std::shared_ptr<TypeSyntax> m_ReturnType;

		std::vector<NameAndTypeSyntax> m_Parameters;

		std::shared_ptr<StatementSyntax> m_Body;
	};

}