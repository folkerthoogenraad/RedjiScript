#include <fstream>

#include "lexer/Lexer.h"

#include "logger/log.h"

int main() {
	using namespace redji;
	
	auto file = std::make_shared<std::ifstream>("test.apx");
	
	Lexer lexer(file);

	auto result = lexer.tokenize();

	for (auto err : lexer.getErrors())
		LOG(err.toString());
	
	for (auto i : result)
		LOG(i);


	WAIT();
}