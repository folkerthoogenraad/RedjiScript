#pragma once

#include <string>
#include <optional>

#include "TypeSyntax.h"

namespace redji {

	class ParameterSyntax : public SyntaxNode{
	public:
		std::string m_Name;
		std::shared_ptr<TypeSyntax> m_Type;

		void accept(SyntaxVisitor &visitor) override { visitor.visit(*this); }
		virtual void toString(std::ostream &stream) const;
	};

}