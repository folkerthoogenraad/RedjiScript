#include <fstream>

#include "lexer/Lexer.h"
#include "parser/Parser.h"
#include "types/SymbolTable.h"

#include "logger/log.h"

#include <vector>
#include <string>
#include <queue>
#include <memory>

#include "types/Type.h"


void lexFile(const std::string &file);
void compileFile(const std::string &file);

// All flags
static bool lexerOnly = false;
static bool verbose = false;
static bool suppressErrors = false;

static std::vector<std::string> files = {};

int main(int argc, char *argv[]) {
	using namespace redji;

	// Simple error message stuff
	if (argc <= 1) {

		LOG("Usage: " << argv[0] << " <files>");
		LOG("use " << argv[0] << " for help");

		WAIT();

		return -1;
	}

	std::vector<std::string> commandlineArgs;

	for (int i = 0; i < argc; i++) {
		commandlineArgs.push_back(argv[i]);
	}

	// Skip first argument (name of the executable)
	size_t index = 1;
	
	for (; index < commandlineArgs.size(); index++) {
		const std::string &current = commandlineArgs[index];

		if (current == "-v" || current == "--verbose") {
			verbose = true;
		}
		else if (current == "-l" || current == "--lexer") {
			lexerOnly = true;
		}
		else if (current == "-noerror" || current == "--suppressErrors") {
			suppressErrors = true;
		}
		else {
			break;
		}
	}

	// Check if there even are files specified
	if (index >= commandlineArgs.size()) {
		LOG("No files specified.");
		return -1;
	}

	// Add all the files specified
	for (; index < commandlineArgs.size(); index++) {
		files.push_back(commandlineArgs[index]);
	}
	
	for (auto &file : files) {
		if (lexerOnly) {
			lexFile(file);
		}
		else {
			compileFile(file);
		}

		
	}


	// Wait here for a while
	WAIT();

	return 0;
}

void lexFile(const std::string &fileName) {
	using namespace redji;

	auto file = std::make_shared<std::ifstream>(fileName);

	Lexer lexer(file);

	auto result = lexer.tokenize();

	if(!suppressErrors)
		for (auto err : lexer.getErrors())
			LOG(err.toString());

	for (auto i : result)
		LOG(i);
}

void compileFile(const std::string &fileName) {
	using namespace redji;

	auto file = std::make_shared<std::ifstream>(fileName);

	Lexer lexer(file);

	auto result = lexer.tokenize();

	if (lexer.getErrors().size() != 0) {
		if (!suppressErrors)
			for (auto err : lexer.getErrors())
				LOG(err.toString());

		// Maybe a flag to not crash or something?
		return;
	}

	Parser parser(result);

	auto parsedUnit = parser.parseAll();

	{
		using namespace compiled;

		auto globalNamespace = std::make_shared<Namespace>("");

		std::queue<std::shared_ptr<CustomType>> types;

		// First look at all the classes
		// Register the available classes
		for (auto stm : parsedUnit->m_Statements) {
			auto cls = std::dynamic_pointer_cast<ClassSyntax>(stm);

			if (cls == nullptr)
				continue;

			auto type = std::make_shared<CustomType>();

			type->m_Name = cls->m_Name.m_Name;
			type->m_Definition = cls;

			types.push(type);

			globalNamespace->addType(type);
		}

		// Set the type for members of types
		while (!types.empty()) {
			auto type = types.front();
			types.pop();

			auto cls = type->m_Definition;

			for (auto m : cls->m_Members) {
				auto member = std::make_shared<NameAndType>();

				member->m_Name = m->m_Name;
				member->m_Type = globalNamespace->findTypeByName(m->m_Type->m_Name);

				type->m_Members.push_back(member);
			}
		}

		// Look at functions next
		for (auto stm : parsedUnit->m_Statements) {
			auto fn = std::dynamic_pointer_cast<FunctionSyntax>(stm);

			if (fn == nullptr)
				continue;

			auto function = std::make_shared<Function>();

			// Get the name
			function->m_Name = fn->m_Name.m_Name;
			
			// Get the return type
			if (fn->m_ReturnType != nullptr) {
				function->m_ReturnType = globalNamespace->findTypeByName(fn->m_ReturnType->m_Name);
			}
			else {
				function->m_ReturnType = BuiltinType::getVoid();
			}

			// Get the parameters
			for (auto& p : fn->m_Parameters) {
				auto parameter = std::make_shared<NameAndType>();

				parameter->m_Name = p.m_Name;
				parameter->m_Type = globalNamespace->findTypeByName(p.m_Type->m_Name);
			
				function->m_Parameters.push_back(parameter);
			}

			globalNamespace->addFunction(function);
		}

		LOG(parsedUnit);
	}
}

