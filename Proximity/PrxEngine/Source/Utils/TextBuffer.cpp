#include "enginepch.h"
#include "Utils/TextBuffer.h"

namespace Proximity::Utils
{
	static bool s_flag = false;
	static bool s_clearNext = false;

	TextBuffer::TextBuffer(Math::U64 streamCount)
		:
		m_current(streamCount - 1),
		m_streamSize(streamCount),
		m_stream(new std::stringstream[streamCount])
	{
		ClearAll();
	}

	TextBuffer::~TextBuffer()
	{
		// No need to manually clear stream. Expected to be used with smart pointer
	}

	void TextBuffer::AddToStream(const std::string& s)
	{
		if (s_clearNext)
		{
			ClearAll();
			s_clearNext = false;
		}

		s_flag = m_current == 0;

		// Clear the string stream and add data
		m_stream[m_current].str(std::string());
		m_stream[m_current] << s << "\n";

		// Update current
		m_current = s_flag ? m_streamSize - 1u : static_cast<Math::U64>(std::floor((m_current - 1) % m_streamSize));
		if (s_flag) 
			s_clearNext = true;
	}

	void TextBuffer::ClearAll()
	{
		// Clear everything until the previous stream message
		for (Math::U64 i = 0; i < m_streamSize; i++)
		{
			m_stream[i].str(std::string());
		}
		m_current = m_streamSize - 1;
	}
}