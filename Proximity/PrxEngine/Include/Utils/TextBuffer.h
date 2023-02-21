#pragma once
namespace Proximity::Utils
{
#ifndef TEXTBUFFER_STREAM_SIZE
#define TEXTBUFFER_STREAM_SIZE 2
#endif

	class TextBuffer
	{
	public:
		// Number of string stream
		TextBuffer(Math::U64 streamSize = 50);
		~TextBuffer();

		void AddToStream(const std::string& s);
		void ClearAll();

		const std::stringstream* GetStreamBuffer() const noexcept { return m_stream; }
		Math::U64 GetStreamSize() { return m_streamSize; }
		Math::U64 GetCurrent() { return m_current; }

	private:
		Math::U64 m_current;
		Math::U64 m_streamSize;
		std::stringstream* m_stream;
	};
}
