#include "Characters.h"

namespace redji {

	Characters::Characters(std::shared_ptr<std::istream> input)
		:m_Input(input), m_Current('\0')
	{
	}

	char Characters::current()
	{
		return m_Current;
	}

	char Characters::next()
	{
		if (m_Current == '\n') {
			m_Line++;
			m_Index = 0;
		}

		m_Input->get(m_Current);

		m_Index++;

		if (!((bool)*this))
			m_Current = '\0';

		return m_Current;
	}

	Characters::operator bool()
	{
		return (bool)m_Input && (bool)*m_Input;
	}

	bool Characters::operator!()
	{
		return !this->operator bool();
	}
}