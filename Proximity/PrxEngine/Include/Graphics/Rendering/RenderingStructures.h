#pragma once
#include "Graphics/Texture2D.h"
#include "Graphics/Rendering/RenderingEnums.h"
#include "Graphics/HandleTypedefs.h"

namespace Proximity::Graphics
{
	using RasterizerState   = ComPtr<ID3D11RasterizerState>;
	using DepthStencilState = ComPtr<ID3D11DepthStencilState>;
	using BlendState        = ComPtr<ID3D11BlendState>;
	using SamplerState      = ComPtr<ID3D11SamplerState>;

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

	struct PipelineState
	{
		ShaderID                 m_Shader;
		BufferID                 m_VertexBuffer;
		BufferID                 m_IndexBuffer;
		D3D11_PRIMITIVE_TOPOLOGY m_Topology;
		RasterizerStateID        m_RasterizerState;
		DepthStencilStateID      m_DepthStencilState;
		BlendStateID             m_BlendState;
		RenderTargetID           m_RenderTarget;  // Maybe use multiple id...vector?
		DepthTargetID            m_DepthTarget;
	};

	struct BufferDesc
	{

	};
}