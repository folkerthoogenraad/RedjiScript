#pragma once

#include <istream>
#include <memory>

namespace redji {

	class Characters {
		std::shared_ptr<std::istream> m_Input;
		char m_Current;

		int m_Line = 1;
		int m_Index = 0;
	public:
		Characters(std::shared_ptr<std::istream> input);

		char current();
		char next();

		inline int line() const { return m_Line; }
		inline int index() const { return m_Index; }

		operator bool();
		bool operator!();
	};

}