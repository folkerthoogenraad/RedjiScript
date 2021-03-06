#pragma once

#include <vector>
#include <memory>

#include "StatementSyntax.h"
#include "TypeSyntax.h"
#include "TypeNameSyntax.h"
#include "MemberSyntax.h"

namespace redji {

	class ClassSyntax : public StatementSyntax {
	public:
		enum {
			Struct, Class
		} m_Type;

		TypeNameSyntax m_Name;

		std::vector<std::shared_ptr<MemberSyntax>> m_Members;

	};

}