#pragma once

#include "SyntaxVisitor.h"

namespace redji {
	class SyntaxNode {
	public:
		virtual void accept(SyntaxVisitor &visitor) { visitor.visit(*this); }
		virtual void toString(std::ostream &stream) const = 0;
	};
}