#pragma once

#include <vector>
#include <memory>

#include "StatementSyntax.h"
#include "TypeSyntax.h"
#include "TypeNameSyntax.h"

namespace redji {

	class ClassSyntax : public StatementSyntax {
	public:
		enum {
			Struct, Class
		} m_Type;

		TypeNameSyntax m_Name;

		std::vector<TypeSyntax> m_Superclasses;

		std::shared_ptr<StatementSyntax> m_Body;
	};

}