#include <fstream>

#include "lexer/Lexer.h"
#include "parser/Parser.h"
#include "types/SymbolTable.h"

#include "logger/log.h"
#include "logger/Logger.h"

#include <vector>
#include <string>
#include <queue>
#include <memory>

#include "types/Symbol.h"

#include "generators/CPPGenerator.h"

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

	SymbolTable table;
	table.process(parsedUnit);

	CPPGenerator cpp;
	std::string s = cpp.generate(*parsedUnit);
}

