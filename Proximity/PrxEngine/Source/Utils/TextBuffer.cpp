#include "enginepch.h"
#include "Utils/TextBuffer.h"

namespace Proximity::Utils
{
	TextBuffer::TextBuffer(Math::U64 streamCount)
		:
		m_current(0),
		m_streamSize(streamCount),
		m_stream(new std::stringstream[streamCount])
	{
		ClearAll();
	}

	TextBuffer::~TextBuffer()
	{
		// No need to manually clear stream. Expected to be used with smart pointer
	}

	void TextBuffer::AddToStream(std::string_view s)
	{
		// Clear the string stream and add data
		m_stream[m_current].str(std::string());
		m_stream[m_current] << s.data() << std::endl;

		// Update current
		m_current = (m_current + 1) % m_streamSize;
		PRX_LOG_INFO("Buffer Current: %u", m_current);
	}

	void TextBuffer::ClearAll()
	{
		// Clear everything until the previous stream message
		for (Math::U64 i = 0; i < m_streamSize; i++)
		{
			m_stream[i].str(std::string());
		}
	}
}