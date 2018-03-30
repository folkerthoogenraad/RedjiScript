#include <fstream>

#include "lexer/Lexer.h"
#include "logger/log.h"

int main() {
	using namespace redji;
	
	auto file = std::make_shared<std::ifstream>("test.apx");
	
	Lexer lexer(file);

	while (lexer.next()) {
		LOG(lexer.current());
	}

	WAIT();
}