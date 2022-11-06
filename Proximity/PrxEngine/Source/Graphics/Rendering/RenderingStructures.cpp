#include "enginepch.h"
#include "Graphics/Rendering/RenderingStructures.h"

namespace Proximity::Graphics
{
	RenderTarget::RenderTarget()
		:
		m_RenderTargetView(nullptr)
	{}
	
	DepthTarget::DepthTarget()
		:
		m_DepthStencilView(nullptr)
	{}
}