#pragma once

#include <string>
#include <vector>

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
		std::vector<TypeSyntax> m_Generics;		// E.x. hans<int>, trudy<int, String>
		std::vector<Array> m_Arrays;			// E.x. hans[][], gerrit[3]
	};

}