#pragma once

#include <optional>
#include <string>
#include <memory>

#include "StatementSyntax.h"
#include "ExpressionSyntax.h"
#include "TypeSyntax.h"

namespace redji {

	class LocalSyntax : public StatementSyntax {
	public:
		std::string m_Name;
		std::shared_ptr<TypeSyntax> m_Type;

		std::shared_ptr<ExpressionSyntax> m_InitialValue;

		void accept(SyntaxVisitor &visitor) override { visitor.visit(*this); }
		virtual void toString(std::ostream &stream) const;
	};

}