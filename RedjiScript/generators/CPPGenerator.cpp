#include "CPPGenerator.h"

#include "ast/AST.h"

namespace redji {
	std::string redji::CPPGenerator::generate(CompilationUnit & node)
	{
		stream.clear();
		node.accept(*this);
	}
}
