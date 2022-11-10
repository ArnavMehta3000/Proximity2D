#pragma once

namespace Proximity::Graphics
{
	using MeshID              = Math::I32;
	using ShaderID            = Math::I32;
	using BufferID            = Math::I32;
	using TextureID           = Math::I32;
	using SamplerID           = Math::I32;
	using RasterizerStateID   = Math::I32;
	using BlendStateID        = Math::I32;
	using DepthStencilStateID = Math::I32;
	using RenderTargetID      = Math::I32;
	using DepthTargetID       = Math::I32;

	constexpr TextureID INVALID_TEXTURE_ID = -1;
	constexpr TextureID INVALID_MATERIAL_ID = -1;
}