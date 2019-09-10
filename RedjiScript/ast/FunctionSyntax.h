#pragma once

#include <optional>
#include <string>
#include <memory>

#include "StatementSyntax.h"
#include "ParameterSyntax.h"
#include "TypeSyntax.h"
#include "TypeNameSyntax.h"

namespace redji {

	class FunctionSyntax : public StatementSyntax {
	public:
		TypeNameSyntax m_Name; // This should 100% be a pointer
		std::shared_ptr<TypeSyntax> m_ReturnType;

		std::vector<std::shared_ptr<ParameterSyntax>> m_Parameters; // This should also be 100% pointers

		std::shared_ptr<StatementSyntax> m_Body;

		void accept(SyntaxVisitor &visitor) override { visitor.visit(*this); }
		virtual void toString(std::ostream &stream) const;
	};

}