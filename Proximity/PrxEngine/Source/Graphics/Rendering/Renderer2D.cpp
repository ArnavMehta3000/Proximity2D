#include "enginepch.h"
#include "Graphics/Rendering/Renderer2D.h"

#define PRX_FAIL_THROW_FUNC(func, msg)\
if (!func)\
{\
	PRX_LOG_ERROR(msg);\
	return false;\
}

namespace Proximity::Graphics
{
	Renderer2D::Renderer2D()
		:
		m_d3d              (nullptr),
		m_depthStencilState(nullptr),
		m_rasterizerStates (std::vector<RasterizerState>  (Defaults::RASTERIZER_STATE_COUNT)),
		m_blendStates      (std::vector<BlendState>       (Defaults::BLEND_STATE_COUNT)),
		m_samplerStates    (std::vector<SamplerState>     (Defaults::SAMPLER_STATE_COUNT)),
		m_resizing         (false)
	{
	}
	Renderer2D::~Renderer2D() {}

	bool Renderer2D::Init(HWND hWnd, Math::U32 width, Math::U32 height, bool isVsync)
	{
		// Initialize DirectX 11
		m_d3d = Core::Globals::g_engineServices.ResolveOrRegisterService<Graphics::D3DManager>();
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

		InitInternal();

		Core::SpriteRendererComponent();  // Create white texture
		if (!Core::VertexTypes::InitBuffers(m_d3d->GetDevice()))
		{
			PRX_LOG_FATAL("Failed create quad vertex/index buffer");
			return false;
		}
		Core::VertexTypes::ApplyQuadBuffers(m_d3d->GetContext());

		Core::Globals::g_resizeEvent += PRX_ACTION_FUNC(Renderer2D::Resize);

		PRX_LOG_DEBUG("Renderer initialized successfully");
		return true;
	}

	void Renderer2D::Shutdown()
	{
		Core::SpriteRendererComponent::Release();
		Core::VertexTypes::ReleaseBuffers();

		// TODO: Do renderer2D shutdown
		COM_RELEASE(m_backBuffer);
		m_editorFrameBuffer.Release();

		// Rasterizer states
		for (auto& r : m_rasterizerStates)
			COM_RELEASE(r);

		// Depth stencil states
		COM_RELEASE(m_depthStencilState);
		m_depthTarget.Release();

		// Blend states
		for (auto& b : m_blendStates)
			COM_RELEASE(b);

		// Sampler states
		for (auto& s : m_samplerStates)
			COM_RELEASE(s);
	}

	void Renderer2D::Resize(Math::U32 width, Math::U32 height)
	{
		m_editorFrameBuffer.Resize(width, height);
		m_depthTarget.Resize(width, height);

		// TODO: Renderer2D resize
		//CreateRenderAndDepthTarget();
		//m_d3d->Resize(width, height);
		/*m_resizing = true;
		m_d3d->GetContext()->OMSetRenderTargets(0, 0, 0);
		
		for (auto& rt : m_renderTarget)
		{
			COM_RELEASE(rt.m_RenderTargetView);
			COM_RELEASE(rt.m_Texture.m_SRV);
			COM_RELEASE(rt.m_Texture.m_Tex2D);
		}

		for (auto& s: m_samplerStates)
			COM_RELEASE(s);

		for (auto& b : m_blendStates)
			COM_RELEASE(b);

		for (auto& d : m_depthStencilStates)
			COM_RELEASE(d);

		for (auto& rs : m_rasterizerStates)
			COM_RELEASE(rs);

		COM_RELEASE(m_depthTarget.m_DepthStencilView);
		COM_RELEASE(m_depthTarget.m_Texture.m_SRV);
		COM_RELEASE(m_depthTarget.m_Texture.m_Tex2D);
		
		HRESULT hr;
		PRX_ASSERT_HR(hr = m_d3d->GetSwapChain()->ResizeBuffers(1, width, height, m_d3d->SwapChainFormat(), 0),
			"Failed to resize buffers");

		InitInternal();
		m_d3d->Resize(width, height);*/
	}

	void Renderer2D::EndFrame()
	{
		// Don't render if reszing
		if (m_resizing)
			return;

		m_d3d->Present();
	}


	void Renderer2D::BindRenderTarget(const RenderTargetType& rtType)
	{
		float color[] = { 0.01f, 0.01f, 0.015f , 1.0f };
		switch (rtType)
		{
		case RenderTargetType::BACK_BUFFER:
			m_d3d->GetContext()->ClearRenderTargetView(m_backBuffer.Get(), color);
			m_d3d->GetContext()->OMSetRenderTargets(1, m_backBuffer.GetAddressOf(), nullptr);
			break;

		case RenderTargetType::FRAME_BUFFER:
			m_d3d->GetContext()->ClearRenderTargetView(m_editorFrameBuffer.RTV.Get(), color);
			m_d3d->GetContext()->OMSetRenderTargets(1, m_editorFrameBuffer.RTV.GetAddressOf(), m_depthTarget.DSV.Get());
			break;

		case RenderTargetType::NONE:
		default:
			m_d3d->GetContext()->OMSetRenderTargets(0, nullptr, m_depthTarget.DSV.Get());
			break;
		}

		m_d3d->GetContext()->ClearDepthStencilView(m_depthTarget.DSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0u);
	}

	void Renderer2D::SetShader(std::shared_ptr<Graphics::GPUShader>& shader, Graphics::GPUShaderType type)
	{
		switch (type)
		{
		case Proximity::Graphics::GPUShaderType::None:
			break;
		case Proximity::Graphics::GPUShaderType::Vertex:
			break;
		case Proximity::Graphics::GPUShaderType::Pixel:
			break;
		default:
			break;
		}
	}

	void Renderer2D::DrawQuad()
	{
		// Assuming quad vertex and index buffers are already set
		m_d3d->GetContext()->DrawIndexed(Core::VertexTypes::QuadIndexCount, 0, 0);
	}



#pragma region Private Initializers
#pragma warning( push )
#pragma warning( disable : 4002 )
	bool Renderer2D::InitInternal()
	{
		PRX_FAIL_THROW_FUNC(CreateRenderAndDepthTarget(), "Failed to create render targets");
		PRX_FAIL_THROW_FUNC(CreateRasterizerStates(),     "Failed to create rasterizer states");
		PRX_FAIL_THROW_FUNC(CreateBlendStates(),          "Failed to create blend states");
		PRX_FAIL_THROW_FUNC(CreateSamplerStates(),        "Failed to create sampler states");

		// Set defaults
		float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		UINT sampleMask = 0xffffffff;
		// Using commands to set the rendder targets
		BindRenderTarget(RenderTargetType::BACK_BUFFER);
		m_d3d->GetContext()->RSSetState(m_rasterizerStates[(int)Defaults::DefaultRasterizerState::CULL_BACK].Get());
		m_d3d->GetContext()->OMSetBlendState(m_blendStates[Defaults::ALPHA_BLEND].Get(), blendFactor, sampleMask);
		m_d3d->GetContext()->OMSetDepthStencilState(m_depthStencilState.Get(), 0);
		
		return true;
	}

	bool Renderer2D::CreateRenderAndDepthTarget()
	{
		HRESULT hr = E_FAIL;
		ComPtr<ID3D11Texture2D> backBufferPtr;
		// Get back buffer texture description
		PRX_ASSERT_HR(hr = m_d3d->GetSwapChain()->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBufferPtr.ReleaseAndGetAddressOf())),
			"Failed to get back buffer");
		PRX_FAIL_HR(hr);

		// Create render targte view
		PRX_ASSERT_HR(hr = m_d3d->GetDevice()->CreateRenderTargetView(backBufferPtr.Get(), nullptr, m_backBuffer.ReleaseAndGetAddressOf()),
			"Failed to create render target from back buffer");
		PRX_FAIL_HR(hr);

		CREATE_ZERO(D3D11_TEXTURE2D_DESC, texDesc);
		backBufferPtr->GetDesc(&texDesc);
		COM_RELEASE(backBufferPtr);

		
		// Init render target texture
		m_editorFrameBuffer                   = RenderTarget();
		m_editorFrameBuffer.RtvFormat         = texDesc.Format;
		m_editorFrameBuffer.Texture.Width     = texDesc.Width;
		m_editorFrameBuffer.Texture.Height    = texDesc.Height;
		m_editorFrameBuffer.Texture.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		m_editorFrameBuffer.Texture.TexFormat = texDesc.Format;
		m_editorFrameBuffer.Texture.SrvFormat = texDesc.Format;
		if (!m_editorFrameBuffer.Texture.CreateTexture())
		{
			PRX_ASSERT_HR(E_FAIL, "Failed to create render target texture from frame buffer");
			return false;
		}

		if (!m_editorFrameBuffer.CreateRTV())
		{
			PRX_ASSERT_HR(E_FAIL, "Failed to create render target view from frame buffer");
			return false;
		}

		if (!m_editorFrameBuffer.Texture.CreateSRV())
		{
			PRX_ASSERT_HR(E_FAIL, "Failed to create shader resource view from frame buffer");
			return false;
		}



		m_depthTarget                   = DepthTarget();
		m_depthTarget.DsvFormat         = DXGI_FORMAT_D24_UNORM_S8_UINT;
		m_depthTarget.Texture.Width     = texDesc.Width;
		m_depthTarget.Texture.Height    = texDesc.Height;
		m_depthTarget.Texture.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		m_depthTarget.Texture.TexFormat = DXGI_FORMAT_R24G8_TYPELESS;
		m_depthTarget.Texture.SrvFormat = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		if (!m_depthTarget.Texture.CreateTexture())
		{
			PRX_ASSERT_HR(E_FAIL, "Failed to create depth buffer texture from depth buffer");
			return false;
		}

		if (!m_depthTarget.CreateDSV())
		{
			PRX_ASSERT_HR(E_FAIL, "Failed to create depth stencil view");
			return false;
		}

		if (!m_editorFrameBuffer.Texture.CreateSRV())
		{
			PRX_ASSERT_HR(E_FAIL, "Failed to create shader resource view from depth buffer");
			return false;
		}

		// Set up the description of the stencil state.
		CREATE_ZERO(D3D11_DEPTH_STENCIL_DESC, depthStencilDesc);
		depthStencilDesc.DepthEnable                  = true;
		depthStencilDesc.DepthWriteMask               = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc                    = D3D11_COMPARISON_LESS_EQUAL;
		depthStencilDesc.StencilEnable                = true;
		depthStencilDesc.StencilReadMask              = 0xFF;
		depthStencilDesc.StencilWriteMask             = 0xFF;
		depthStencilDesc.FrontFace.StencilFailOp      = D3D11_STENCIL_OP_KEEP;  // Stencil operations if pixel is front-facing.
		depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		depthStencilDesc.FrontFace.StencilPassOp      = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilFunc        = D3D11_COMPARISON_ALWAYS;
		depthStencilDesc.BackFace.StencilFailOp       = D3D11_STENCIL_OP_KEEP;  // Stencil operations if pixel is back-facing.
		depthStencilDesc.BackFace.StencilDepthFailOp  = D3D11_STENCIL_OP_DECR;
		depthStencilDesc.BackFace.StencilPassOp       = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilFunc         = D3D11_COMPARISON_ALWAYS;
		hr = m_d3d->GetDevice()->CreateDepthStencilState(&depthStencilDesc, m_depthStencilState.ReleaseAndGetAddressOf());
		PRX_FAIL_HR(hr);

		return true;
	}

	bool Renderer2D::CreateRasterizerStates()
	{
		HRESULT hr = E_FAIL;
		const std::string err("Unable to create Rasterizer State: Cull ");

		CREATE_ZERO(D3D11_RASTERIZER_DESC, rsDesc);

		rsDesc.FillMode              = D3D11_FILL_SOLID;
		rsDesc.FrontCounterClockwise = false;
		rsDesc.DepthBias             = 0;
		rsDesc.ScissorEnable         = false;
		rsDesc.DepthBiasClamp        = 0;
		rsDesc.SlopeScaledDepthBias  = 0.0f;
		rsDesc.DepthClipEnable       = true;
		rsDesc.AntialiasedLineEnable = true;
		rsDesc.MultisampleEnable     = false;

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
		rtBlendDesc.BlendOp               = D3D11_BLEND_OP_ADD;
		rtBlendDesc.SrcBlend              = D3D11_BLEND_ONE;
		rtBlendDesc.DestBlend             = D3D11_BLEND_ONE;
		rtBlendDesc.BlendOpAlpha          = D3D11_BLEND_OP_MIN;
		rtBlendDesc.SrcBlendAlpha         = D3D11_BLEND_SRC_ALPHA;
		rtBlendDesc.DestBlendAlpha        = D3D11_BLEND_DEST_ALPHA;
		rtBlendDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		CREATE_ZERO(D3D11_BLEND_DESC, desc);
		desc.RenderTarget[0] = rtBlendDesc;

		PRX_ASSERT_HR(hr = m_d3d->GetDevice()->CreateBlendState(&desc, m_blendStates[Defaults::ADDITIVE_COLOR].ReleaseAndGetAddressOf()),
			"Faied to create blend state: ADDITIVE COLOR");
		PRX_FAIL_HR(hr);

		rtBlendDesc.BlendOp               = D3D11_BLEND_OP_ADD;
		rtBlendDesc.SrcBlend              = D3D11_BLEND_SRC_ALPHA;
		rtBlendDesc.DestBlend             = D3D11_BLEND_INV_SRC_ALPHA;
		rtBlendDesc.BlendOpAlpha          = D3D11_BLEND_OP_ADD;
		rtBlendDesc.SrcBlendAlpha         = D3D11_BLEND_ONE;
		rtBlendDesc.DestBlendAlpha        = D3D11_BLEND_ZERO;
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
#pragma warning( pop )  
#pragma endregion

}