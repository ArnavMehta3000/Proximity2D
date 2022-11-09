#include "enginepch.h"
#include "Graphics/Rendering/Renderer2D.h"

namespace Proximity::Graphics
{
	Renderer2D::Renderer2D()
		:
		m_d3d(nullptr),
		m_rasterizerStates  (std::vector<RasterizerState>  (Defaults::RASTERIZER_STATE_COUNT)),
		m_depthStencilStates(std::vector<DepthStencilState>(Defaults::DEPTH_STENCIL_STATE_COUNT)),
		m_blendStates       (std::vector<BlendState>       (Defaults::BLEND_STATE_COUNT)),
		m_samplerStates     (std::vector<SamplerState>     (Defaults::SAMPLER_STATE_COUNT))
	{
	}
	Renderer2D::~Renderer2D() {}

	bool Renderer2D::Init(HWND hWnd, Math::U32 width, Math::U32 height, bool isVsync)
	{
		// Initialize DirectX 11
		m_d3d = Core::g_engineServices.ResolveOrRegisterService<Graphics::D3DManager>();
		if (!m_d3d)
		{
			PRX_LOG_FATAL("Failed create D3DManager");
			return false;
		}

		if (!m_d3d->Init(width, height, isVsync, hWnd, DXGI_FORMAT_R16G16B16A16_FLOAT))
		{
			PRX_LOG_FATAL("Failed to initialize DirectX 11");
			return false;
		}

		if (!CreateRenderTargets())
		{
			PRX_LOG_ERROR("Failed to create render targets");
			return false;
		}

		if (!CreateRasterizerStates())
		{
			PRX_LOG_ERROR("Failed to create rasterizer states");
			return false;
		}

		if (!CreateDepthStencilStates())
		{
			PRX_LOG_ERROR("Failed to create depth stencil states");
			return false;
		}

		if (!CreateBlendStates())
		{
			PRX_LOG_ERROR("Failed to create blend states");
			return false;
		}

		if (!CreateSamplerStates())
		{
			PRX_LOG_ERROR("Failed to create sampler states");
			return false;
		}


		return true;
	}

	void Renderer2D::Shutdown()
	{
		// TODO: Do renderer2D shutdown

		for (auto& r : m_renderTargets)
			COM_RELEASE(r.m_RenderTargetView);

		// Rasterizer states
		for (auto& r : m_rasterizerStates)
			COM_RELEASE(r);

		// Depth stencil states
		for (auto& d : m_depthStencilStates)
			COM_RELEASE(d);

		// Blend states
		for (auto& b : m_blendStates)
			COM_RELEASE(b);

		// Sampler states
		for (auto& s : m_samplerStates)
			COM_RELEASE(s);
	}

	void Renderer2D::BeginRendering(const ClearCommand& clrCmd)
	{
		// Clear render targets
		if (clrCmd.m_ShouldClearColor)
		{
			// TODO: Set up for multiple rendering targets
			for (auto& r : m_renderTargets)
				m_d3d->GetContext()->ClearRenderTargetView(r.m_RenderTargetView.Get(), clrCmd.m_ClearColor.data());
		}

		// Clear depth and stencil based on flag
		if (clrCmd.m_ShouldClearDepth || clrCmd.m_ShouldClearStencil)
		{
			U32 clearFlags = [&]() -> U32  // Quick lambda used to get the DX clear flag
			{
				if (clrCmd.m_ShouldClearDepth && clrCmd.m_ShouldClearStencil)
					return D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL;

				if (clrCmd.m_ShouldClearDepth)
					return D3D11_CLEAR_DEPTH;

				return D3D11_CLEAR_STENCIL;
			}();

			m_d3d->GetContext()->ClearDepthStencilView(m_depthTarget.m_DepthStencilView.Get(), clearFlags, clrCmd.m_ClearDepth, clrCmd.m_ClearStencil);
		}
	}

	void Renderer2D::BeginFrame()
	{
	}

	void Renderer2D::EndFrame()
	{
		m_d3d->Present();
	}

#pragma region Private Initializers
#pragma warning( push )
#pragma warning( disable : 4002 )
	bool Renderer2D::CreateRenderTargets()
	{
		RenderTarget backBufferRT;
		ComPtr<ID3D11Texture2D> backBuffer = nullptr;
		
		HRESULT hr = E_FAIL;
		PRX_ASSERT_HR(hr = m_d3d->GetSwapChain()->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)backBuffer.ReleaseAndGetAddressOf()),
			"Failed to get back buffer");
		PRX_FAIL_HR(hr);

		// Get back buffer texture description
		CREATE_ZERO(D3D11_TEXTURE2D_DESC, texDesc);
		backBuffer->GetDesc(&texDesc);

		// Create shader resource view description
		CREATE_ZERO(D3D11_SHADER_RESOURCE_VIEW_DESC, srvDesc);
		srvDesc.Format                    = texDesc.Format;
		srvDesc.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels       = 1;
		srvDesc.Texture2D.MostDetailedMip = 0;
		hr = m_d3d->GetDevice()->CreateShaderResourceView(backBuffer.Get(), &srvDesc, backBufferRT.m_Texture.m_SRV.ReleaseAndGetAddressOf());
		PRX_FAIL_HR(hr);

		backBufferRT.m_Texture.m_Tex2D = backBuffer;
		hr = m_d3d->GetDevice()->CreateRenderTargetView(backBuffer.Get(), nullptr, backBufferRT.m_RenderTargetView.ReleaseAndGetAddressOf());
		PRX_FAIL_HR(hr);

		m_renderTargets.push_back(backBufferRT);

		return true;
	}

	bool Renderer2D::CreateRasterizerStates()
	{
		HRESULT hr = E_FAIL;
		const std::string err("Unable to create Rasterizer State: Cull ");

		CREATE_ZERO(D3D11_RASTERIZER_DESC, rsDesc);

		rsDesc.FillMode = D3D11_FILL_SOLID;
		rsDesc.FrontCounterClockwise = false;
		rsDesc.DepthBias = 0;
		rsDesc.ScissorEnable = false;
		rsDesc.DepthBiasClamp = 0;
		rsDesc.SlopeScaledDepthBias = 0.0f;
		rsDesc.DepthClipEnable = true;
		rsDesc.AntialiasedLineEnable = true;
		rsDesc.MultisampleEnable = false;

		rsDesc.CullMode = D3D11_CULL_BACK;
		PRX_ASSERT_HR(hr = m_d3d->GetDevice()->CreateRasterizerState(&rsDesc, m_rasterizerStates[(int)Defaults::DefaultRasterizerState::CULL_BACK].ReleaseAndGetAddressOf()),
			"%sBack", err);
		PRX_FAIL_HR(hr);

		rsDesc.CullMode = D3D11_CULL_FRONT;
		PRX_ASSERT_HR(hr = m_d3d->GetDevice()->CreateRasterizerState(&rsDesc, m_rasterizerStates[(int)Defaults::DefaultRasterizerState::CULL_FRONT].ReleaseAndGetAddressOf()),
			"%sFront", err);
		PRX_FAIL_HR(hr);

		rsDesc.CullMode = D3D11_CULL_NONE;
		PRX_ASSERT_HR(hr = m_d3d->GetDevice()->CreateRasterizerState(&rsDesc, m_rasterizerStates[(int)Defaults::DefaultRasterizerState::CULL_NONE].ReleaseAndGetAddressOf()),
			"%sNone", err);
		PRX_FAIL_HR(hr);

		rsDesc.FillMode = static_cast<D3D11_FILL_MODE>(RasterizerFillMode::Wireframe);
		PRX_ASSERT_HR(hr = m_d3d->GetDevice()->CreateRasterizerState(&rsDesc, m_rasterizerStates[(int)Defaults::DefaultRasterizerState::WIREFRAME].ReleaseAndGetAddressOf()),
			"%sWireframe", err);
		PRX_FAIL_HR(hr);

		PRX_LOG_DEBUG("Created all rasterizer states");
		return true;
	}

	bool Renderer2D::CreateBlendStates()
	{
		HRESULT hr = E_FAIL;

		CREATE_ZERO(D3D11_RENDER_TARGET_BLEND_DESC, rtBlendDesc);
		rtBlendDesc.BlendEnable = true;
		rtBlendDesc.BlendOp = D3D11_BLEND_OP_ADD;
		rtBlendDesc.SrcBlend = D3D11_BLEND_ONE;
		rtBlendDesc.DestBlend = D3D11_BLEND_ONE;
		rtBlendDesc.BlendOpAlpha = D3D11_BLEND_OP_MIN;
		rtBlendDesc.SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
		rtBlendDesc.DestBlendAlpha = D3D11_BLEND_DEST_ALPHA;
		rtBlendDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		CREATE_ZERO(D3D11_BLEND_DESC, desc);
		desc.RenderTarget[0] = rtBlendDesc;

		PRX_ASSERT_HR(hr = m_d3d->GetDevice()->CreateBlendState(&desc, m_blendStates[Defaults::ADDITIVE_COLOR].ReleaseAndGetAddressOf()),
			"Faied to create blend state: ADDITIVE COLOR");
		PRX_FAIL_HR(hr);

		rtBlendDesc.BlendOp = D3D11_BLEND_OP_ADD;
		rtBlendDesc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
		rtBlendDesc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		rtBlendDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
		rtBlendDesc.SrcBlendAlpha = D3D11_BLEND_ONE;
		rtBlendDesc.DestBlendAlpha = D3D11_BLEND_ZERO;
		rtBlendDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		desc.RenderTarget[0] = rtBlendDesc;

		PRX_ASSERT_HR(hr = m_d3d->GetDevice()->CreateBlendState(&desc, m_blendStates[Defaults::ALPHA_BLEND].ReleaseAndGetAddressOf()),
			"Faied to create blend state: ALPHA BLEND");
		PRX_FAIL_HR(hr);

		rtBlendDesc.BlendEnable = false;
		desc.RenderTarget[0] = rtBlendDesc;

		PRX_ASSERT_HR(hr = m_d3d->GetDevice()->CreateBlendState(&desc, m_blendStates[Defaults::DISABLED].ReleaseAndGetAddressOf()),
			"Faied to create blend state: DISABLED");
		PRX_FAIL_HR(hr);


		PRX_LOG_DEBUG("Created all blend states");
		return true;
	}

	bool Renderer2D::CreateSamplerStates()
	{
		HRESULT hr = E_FAIL;

		CREATE_ZERO(D3D11_SAMPLER_DESC, samplerDesc);

		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		PRX_ASSERT_HR(hr = m_d3d->GetDevice()->CreateSamplerState(&samplerDesc, m_samplerStates[Defaults::WRAP_SAMPLER].ReleaseAndGetAddressOf()),
			"Failed to create sampler state: WRAP SAMPLER");
		PRX_FAIL_HR(hr);

		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		PRX_ASSERT_HR(hr = m_d3d->GetDevice()->CreateSamplerState(&samplerDesc, m_samplerStates[Defaults::POINT_SAMPLER].ReleaseAndGetAddressOf()),
			"Failed to create sampler state: POINT SAMPLER");
		PRX_FAIL_HR(hr);

		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
		PRX_ASSERT_HR(hr = m_d3d->GetDevice()->CreateSamplerState(&samplerDesc, m_samplerStates[Defaults::LINEAR_FILTER_SAMPLER_WRAP_UVW].ReleaseAndGetAddressOf()),
			"Failed to create sampler state: LINEAR WRAP SAMPLER");
		PRX_FAIL_HR(hr);

		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		PRX_ASSERT_HR(hr = m_d3d->GetDevice()->CreateSamplerState(&samplerDesc, m_samplerStates[Defaults::LINEAR_FILTER_SAMPLER].ReleaseAndGetAddressOf()),
			"Failed to create sampler state: LINEAR SAMPLER");
		PRX_FAIL_HR(hr);

		samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.MaxAnisotropy = 1;
		PRX_ASSERT_HR(hr = m_d3d->GetDevice()->CreateSamplerState(&samplerDesc, m_samplerStates[Defaults::ANISOTROPIC_1_CLAMPED_SAMPLER].ReleaseAndGetAddressOf()),
			"Failed to create sampler state: ANISOTROPIC 1 CLAMPED SAMPLER");
		PRX_FAIL_HR(hr);

		samplerDesc.MaxAnisotropy = 2;
		PRX_ASSERT_HR(hr = m_d3d->GetDevice()->CreateSamplerState(&samplerDesc, m_samplerStates[Defaults::ANISOTROPIC_2_CLAMPED_SAMPLER].ReleaseAndGetAddressOf()),
			"Failed to create sampler state: ANISOTROPIC 2 CLAMPED SAMPLER");
		PRX_FAIL_HR(hr);

		samplerDesc.MaxAnisotropy = 4;
		PRX_ASSERT_HR(hr = m_d3d->GetDevice()->CreateSamplerState(&samplerDesc, m_samplerStates[Defaults::ANISOTROPIC_4_CLAMPED_SAMPLER].ReleaseAndGetAddressOf()),
			"Failed to create sampler state: ANISOTROPIC 4 CLAMPED SAMPLER");
		PRX_FAIL_HR(hr);

		samplerDesc.MaxAnisotropy = 16;
		PRX_ASSERT_HR(hr = m_d3d->GetDevice()->CreateSamplerState(&samplerDesc, m_samplerStates[Defaults::ANISOTROPIC_16_CLAMPED_SAMPLER].ReleaseAndGetAddressOf()),
			"Failed to create sampler state: ANISOTROPIC 16 CLAMPED SAMPLER");
		PRX_FAIL_HR(hr);

		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.MaxAnisotropy = 1;
		PRX_ASSERT_HR(hr = m_d3d->GetDevice()->CreateSamplerState(&samplerDesc, m_samplerStates[Defaults::ANISOTROPIC_1_WRAPPED_SAMPLER].ReleaseAndGetAddressOf()),
			"Failed to create sampler state: ANISOTROPIC 1 WRAPPED SAMPLER");
		PRX_FAIL_HR(hr);

		samplerDesc.MaxAnisotropy = 2;
		PRX_ASSERT_HR(hr = m_d3d->GetDevice()->CreateSamplerState(&samplerDesc, m_samplerStates[Defaults::ANISOTROPIC_2_WRAPPED_SAMPLER].ReleaseAndGetAddressOf()),
			"Failed to create sampler state: ANISOTROPIC 2 WRAPPED SAMPLER");
		PRX_FAIL_HR(hr);

		samplerDesc.MaxAnisotropy = 4;
		PRX_ASSERT_HR(hr = m_d3d->GetDevice()->CreateSamplerState(&samplerDesc, m_samplerStates[Defaults::ANISOTROPIC_4_WRAPPED_SAMPLER].ReleaseAndGetAddressOf()),
			"Failed to create sampler state: ANISOTROPIC 4 WRAPPED SAMPLER");
		PRX_FAIL_HR(hr);

		samplerDesc.MaxAnisotropy = 16;
		PRX_ASSERT_HR(hr = m_d3d->GetDevice()->CreateSamplerState(&samplerDesc, m_samplerStates[Defaults::ANISOTROPIC_16_WRAPPED_SAMPLER].ReleaseAndGetAddressOf()),
			"Failed to create sampler state: ANISOTROPIC 16 WRAPPED SAMPLER");
		PRX_FAIL_HR(hr);

		PRX_LOG_DEBUG("Created all sampler states");

		return true;
	}

	bool Renderer2D::CreateDepthStencilStates()
	{
		HRESULT hr = E_FAIL;

		CREATE_ZERO(D3D11_DEPTH_STENCIL_DESC, depthStencilDesc);
		// Set up the description of the stencil state.
		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		depthStencilDesc.StencilEnable = true;
		depthStencilDesc.StencilReadMask = 0xFF;
		depthStencilDesc.StencilWriteMask = 0xFF;
		// Stencil operations if pixel is front-facing.
		depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		// Stencil operations if pixel is back-facing.
		depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		PRX_ASSERT_HR(hr = m_d3d->GetDevice()->CreateDepthStencilState(&depthStencilDesc, m_depthStencilStates[Defaults::DEPTH_STENCIL_WRITE].ReleaseAndGetAddressOf()),
			"Failed to create depth stencil state: DEPTH & STENCIL WRITE");
		PRX_FAIL_HR(hr);

		depthStencilDesc.DepthEnable = false;
		depthStencilDesc.StencilEnable = false;
		PRX_ASSERT_HR(hr = m_d3d->GetDevice()->CreateDepthStencilState(&depthStencilDesc, m_depthStencilStates[Defaults::DEPTH_STENCIL_DISABLED].ReleaseAndGetAddressOf()),
			"Failed to create depth stencil state: DEPTH & STENCIL DISABLED");
		PRX_FAIL_HR(hr);

		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.StencilEnable = false;
		PRX_ASSERT_HR(hr = m_d3d->GetDevice()->CreateDepthStencilState(&depthStencilDesc, m_depthStencilStates[Defaults::DEPTH_WRITE].ReleaseAndGetAddressOf()),
			"Failed to create depth stencil state: DEPTH WRITE");
		PRX_FAIL_HR(hr);

		depthStencilDesc.DepthEnable = false;
		depthStencilDesc.StencilEnable = true;
		PRX_ASSERT_HR(hr = m_d3d->GetDevice()->CreateDepthStencilState(&depthStencilDesc, m_depthStencilStates[Defaults::STENCIL_WRITE].ReleaseAndGetAddressOf()),
			"Failed to create depth stencil state: STENCIL WRITE");
		PRX_FAIL_HR(hr);

		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		depthStencilDesc.StencilEnable = false;
		PRX_ASSERT_HR(hr = m_d3d->GetDevice()->CreateDepthStencilState(&depthStencilDesc, m_depthStencilStates[Defaults::DEPTH_TEST_ONLY].ReleaseAndGetAddressOf()),
			"Failed to create depth stencil state: DEPTH TEST ONLY");
		PRX_FAIL_HR(hr);

		PRX_LOG_DEBUG("Created all depth stencil states");
		return true;
	}
#pragma warning( pop )  
#pragma endregion

}