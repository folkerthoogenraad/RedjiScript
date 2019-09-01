#pragma once

#include <optional>
#include <string>
#include <memory>

#include "StatementSyntax.h"
#include "ExpressionSyntax.h"
#include "NameAndTypeSyntax.h"
#include "TypeSyntax.h"

namespace redji {

	class LocalSyntax : public StatementSyntax {
	public:
		std::string m_Name;
		std::shared_ptr<TypeSyntax> m_Type;

		std::shared_ptr<ExpressionSyntax> m_InitialValue;
	};

}