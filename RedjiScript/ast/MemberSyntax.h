#pragma once

#include <string>
#include <memory>
#include <optional>

#include "StatementSyntax.h"
#include "ExpressionSyntax.h"
#include "NameAndTypeSyntax.h"
#include "TypeSyntax.h"

namespace redji {

	class MemberSyntax : public StatementSyntax {
	public:
		std::string m_Name;

		// Can become an optional sometime
		std::shared_ptr<TypeSyntax> m_Type;
		
		// Not yet implemented.
		std::shared_ptr<ExpressionSyntax> m_InitialValue;
	};

}