#pragma once

#include <string>
#include <vector>

#include "StatementSyntax.h"

namespace redji {

	class TypeSyntax : public StatementSyntax{
	public:
		struct Array {
			enum Type{Stack, Heap} type;		// Type of array
			size_t size;						// Only for stack arrays
		public:
			Array(Type type = Stack, size_t size = 0) : type(type), size(size) {};
		};
	public:
		std::string m_Name;						// E.x. hans, trudy, gerrit
		std::vector<std::shared_ptr<TypeSyntax>> m_Generics;		// E.x. hans<int>, trudy<int, String>
		std::vector<Array> m_Arrays;			// E.x. hans[][], gerrit[3]

		void accept(SyntaxVisitor &visitor) override { visitor.visit(*this); }
		virtual void toString(std::ostream &stream) const;
	};

}