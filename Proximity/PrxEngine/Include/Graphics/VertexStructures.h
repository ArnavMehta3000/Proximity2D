#pragma once

namespace Proximity::Core::VertexTypes
{
	struct TexturedVertex
	{
		DirectX::XMFLOAT3 Position;
		DirectX::XMFLOAT2 TexCoord;
	};

	namespace
	{
		inline static TexturedVertex QuadVertices[] =
		{
			{ DirectX::XMFLOAT3(-0.5f, 0.5f, 0.0f), DirectX::XMFLOAT2(0.0f, 0.0f) },
			{ DirectX::XMFLOAT3(0.5f, 0.5f, 0.0f), DirectX::XMFLOAT2(1.0f, 0.0f) },
			{ DirectX::XMFLOAT3(-0.5f,-0.5f, 0.0f), DirectX::XMFLOAT2(0.0f, 1.0f) },
			{ DirectX::XMFLOAT3(0.5f,-0.5f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f) },
		};

		inline static DWORD QuadIndices[] = { 0, 1, 2, 2, 1, 3 };
		inline static UINT QuadStride = (UINT)sizeof(TexturedVertex);
		inline static UINT QuadOffset = 0;
		inline static UINT QuadIndexCount = ARRAYSIZE(QuadIndices);
		inline static UINT QuadVertexCount = ARRAYSIZE(QuadVertices);
	}

	inline static ComPtr<ID3D11Buffer> s_quadVertexBuffer;
	inline static ComPtr<ID3D11Buffer> s_quadIndexBuffer;

	inline static bool InitBuffers(ID3D11Device* device)
	{
		// ----- Create Vertex Buffer -----
		CREATE_ZERO(D3D11_BUFFER_DESC, vbd);
		vbd.Usage          = D3D11_USAGE_DEFAULT;
		vbd.ByteWidth      = sizeof(TexturedVertex) * QuadVertexCount;
		vbd.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
		vbd.CPUAccessFlags = 0;
		vbd.MiscFlags      = 0;

		CREATE_ZERO(D3D11_SUBRESOURCE_DATA, vsd);
		vsd.pSysMem = QuadVertices;

		PRX_ASSERT_HR(device->CreateBuffer(&vbd, &vsd, s_quadVertexBuffer.ReleaseAndGetAddressOf()), "Failed to create quad vertex buffer");
		// ----- Create Vertex Buffer -----



		// ----- Create Index Buffer -----
		CREATE_ZERO(D3D11_BUFFER_DESC, ibd);
		ibd.Usage          = D3D11_USAGE_DEFAULT;
		ibd.ByteWidth      = sizeof(DWORD) * QuadIndexCount;
		ibd.BindFlags      = D3D11_BIND_INDEX_BUFFER;
		ibd.CPUAccessFlags = 0;
		ibd.MiscFlags      = 0;

		CREATE_ZERO(D3D11_SUBRESOURCE_DATA, isd);
		isd.pSysMem = QuadIndices;

		PRX_ASSERT_HR(device->CreateBuffer(&ibd, &isd, s_quadIndexBuffer.ReleaseAndGetAddressOf()), "Failed to create quad index buffer");
		// ----- Create Index Buffer -----

		PRX_LOG_DEBUG("Successfully created quad index buffers");

		return true;
	}

	inline static void ApplyQuadBuffers(ID3D11DeviceContext* context)
	{
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		context->IASetVertexBuffers(0u, 1u, s_quadVertexBuffer.GetAddressOf(), &QuadStride, &QuadOffset);
		context->IASetIndexBuffer(s_quadIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	}

	inline static void ReleaseBuffers()
	{
		COM_RELEASE(s_quadVertexBuffer);
		COM_RELEASE(s_quadIndexBuffer);
	}
}