#pragma once


namespace Proximity::Graphics
{
	struct ClearCommand
	{
		static ClearCommand Depth(Math::F32 depthClear);
		static ClearCommand Color(Math::F32 r, Math::F32 g, Math::F32 b, Math::F32 a = 1.0f);

		ClearCommand(bool shouldClearColor, bool shouldClearDepth, bool shouldClearStencil,
			const std::array<Math::F32, 4> clearColor, Math::F32 depthClear, unsigned char clearStencil);

		bool                     m_ShouldClearColor;
		bool                     m_ShouldClearDepth;
		bool                     m_ShouldClearStencil;
		std::array<Math::F32, 4> m_ClearColor;
		Math::F32                m_ClearDepth;
		unsigned char            m_ClearStencil;
	};
}