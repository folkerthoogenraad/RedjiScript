#pragma once

#include <string>
#include <vector>

#include "StatementSyntax.h"

namespace redji {

	class TypeNameSyntax : public StatementSyntax{
	public:
		std::string m_Name;
		std::vector<std::string> m_GenericNames;

		void accept(SyntaxVisitor &visitor) override { visitor.visit(*this); }
		virtual void toString(std::ostream &stream) const;
	};

}