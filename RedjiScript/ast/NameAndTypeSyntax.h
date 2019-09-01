#pragma once

#include <string>
#include <optional>

#include "TypeSyntax.h"

namespace redji {

	class NameAndTypeSyntax {
	public:
		std::string m_Name;
		std::shared_ptr<TypeSyntax> m_Type;
	};

}