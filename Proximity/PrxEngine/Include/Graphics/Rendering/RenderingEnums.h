#pragma once
#include "Graphics/Rendering/RenderingDefaults.h"
#include <d3d11.h>

namespace Proximity::Graphics
{
	enum class RasterizerCullMode
	{
		Front     = D3D11_CULL_FRONT,
		None      = D3D11_CULL_NONE,
		Back      = D3D11_CULL_BACK,
	};

	enum class RasterizerFillMode
	{
		Solid     = D3D11_FILL_SOLID,
		Wireframe = D3D11_FILL_WIREFRAME,
	};

	enum class RenderTargetType
	{
		NONE = -1,
		BACK_BUFFER,
		FRAME_BUFFER
	};
}