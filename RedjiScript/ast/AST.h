#pragma once

#include "BlockSyntax.h"
#include "ClassSyntax.h"
#include "CompilationUnit.h"
#include "ExpressionStatementSyntax.h"
#include "ExpressionSyntax.h"
#include "FunctionSyntax.h"
#include "LocalSyntax.h"
#include "MemberSyntax.h"
#include "ParameterSyntax.h"
#include "ReturnSyntax.h"
#include "StatementSyntax.h"
#include "SyntaxNode.h"
#include "TypeNameSyntax.h"
#include "TypeSyntax.h"

#include <ostream>

namespace redji {

	std::ostream& operator<<(std::ostream& os, const SyntaxNode& dt);
}