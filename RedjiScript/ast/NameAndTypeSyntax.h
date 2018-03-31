#pragma once

#include <string>
#include <optional>

#include "TypeSyntax.h"

namespace redji {

	class NameAndTypeSyntax {
	public:
		std::string m_Name;
		std::optional<TypeSyntax> m_Type; // TODO this should be type syntax, whatever
	};

}