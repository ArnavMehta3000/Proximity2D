#pragma once
#include "Graphics/Texture2D.h"

namespace Proximity::Graphics
{
	struct RenderTarget
	{
		RenderTarget();

		Texture2D                      m_Texture;
		ComPtr<ID3D11RenderTargetView> m_RenderTargetView;
	};

	struct DepthTarget
	{
		DepthTarget();
		Texture2D                      m_Texture;
		ComPtr<ID3D11DepthStencilView> m_DepthStencilView;
	};
}