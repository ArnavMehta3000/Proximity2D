#include "enginepch.h"
#include "Graphics/Rendering/RenderCommands.h"

namespace Proximity::Graphics
{
	ClearCommand ClearCommand::Depth(F32 depthClear)
	{
		return ClearCommand(false, true, false, {0.0f, 0.0f, 0.0f, 0.0f}, depthClear, 0);
	}

	ClearCommand ClearCommand::Color(F32 r, F32 g, F32 b, F32 a)
	{
		return ClearCommand(true, false, false, { r, g, b, a }, 0, 0);
	}

	ClearCommand::ClearCommand(bool shouldClearColor, bool shouldClearDepth, bool shouldClearStencil, const std::array<Math::F32, 4> clearColor, Math::F32 depthClear, unsigned char clearStencil)
		:
		m_ShouldClearColor(shouldClearColor),
		m_ShouldClearDepth(shouldClearDepth),
		m_ShouldClearStencil(shouldClearStencil),
		m_ClearColor(clearColor),
		m_ClearDepth(depthClear),
		m_ClearStencil(clearStencil)
	{
	}
}