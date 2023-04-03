#include "enginepch.h"
#include "Graphics/Rendering/BatchRenderer.h"

namespace Proximity::Graphics
{
	static const size_t MaxQuadCount   = 1000;
	static const size_t MaxVertexCount = MaxQuadCount * 4;
	static const size_t MaxIndexCount  = MaxQuadCount * 6;
	static const size_t MaxTextures    = 32;


	void BatchRenderer::Init()
	{
	}

	void BatchRenderer::Shutdown()
	{
	}

	void BatchRenderer::BeginBatch()
	{
	}

	void BatchRenderer::EndBatch()
	{
	}

	void BatchRenderer::Flush()
	{
	}

	void BatchRenderer::DrawQaud(const Math::Matrix& worldMatrix, const DirectX::XMFLOAT4& color)
	{
	}

	const BatchRenderer::Stats& BatchRenderer::GetStats()
	{
		// TODO: insert return statement here
	}
}