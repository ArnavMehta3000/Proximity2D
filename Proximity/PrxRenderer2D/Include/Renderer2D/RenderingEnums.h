#pragma once
#include "Renderer2D/RenderingDefaults.h"
#include <d3d11.h>

namespace Proximity::Graphics
{
	using RasterizerState   = ComPtr<ID3D11RasterizerState>;
	using DepthStencilState = ComPtr<ID3D11DepthStencilState>;
	using BlendState        = ComPtr<ID3D11BlendState>;
	using SamplerState      = ComPtr<ID3D11SamplerState>;
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

}