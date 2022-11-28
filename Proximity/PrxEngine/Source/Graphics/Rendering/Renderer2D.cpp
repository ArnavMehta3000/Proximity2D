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

		Core::Globals::g_resizeEvent += PRX_ACTION_FUNC(Renderer2D::Resize);

		PRX_LOG_DEBUG("Renderer initialized successfully");
		return true;
	}

	void Renderer2D::Shutdown()
	{
		// TODO: Do renderer2D shutdown
		COM_RELEASE(m_backBuffer.m_RenderTargetView);
		COM_RELEASE(m_backBuffer.m_Texture.m_SRV);
		COM_RELEASE(m_backBuffer.m_Texture.m_Tex2D);

		COM_RELEASE(m_frameBuffer.m_RenderTargetView);
		COM_RELEASE(m_frameBuffer.m_Texture.m_SRV);
		COM_RELEASE(m_frameBuffer.m_Texture.m_Tex2D);

		// Rasterizer states
		for (auto& r : m_rasterizerStates)
			COM_RELEASE(r);

		// Depth stencil states
		COM_RELEASE(m_depthStencilState);
		COM_RELEASE(m_depthTarget.m_DepthStencilView);
		COM_RELEASE(m_depthTarget.m_Texture.m_SRV);
		COM_RELEASE(m_depthTarget.m_Texture.m_Tex2D);

		// Blend states
		for (auto& b : m_blendStates)
			COM_RELEASE(b);

		// Sampler states
		for (auto& s : m_samplerStates)
			COM_RELEASE(s);
	}

	void Renderer2D::Resize(Math::U32 width, Math::U32 height)
	{
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
		float color[] = { 0, 0, 0 , 1.0f };
		switch (rtType)
		{
		case RenderTargetType::BACK_BUFFER:
			m_d3d->GetContext()->ClearRenderTargetView(m_backBuffer.m_RenderTargetView.Get(), color);
			m_d3d->GetContext()->OMSetRenderTargets(1, m_backBuffer.m_RenderTargetView.GetAddressOf(), m_depthTarget.m_DepthStencilView.Get());
			break;

		case RenderTargetType::FRAME_BUFFER:
			m_d3d->GetContext()->ClearRenderTargetView(m_frameBuffer.m_RenderTargetView.Get(), color);
			m_d3d->GetContext()->OMSetRenderTargets(1, m_frameBuffer.m_RenderTargetView.GetAddressOf(), m_depthTarget.m_DepthStencilView.Get());
			break;

		case RenderTargetType::NONE:
		default:
			m_d3d->GetContext()->OMSetRenderTargets(0, nullptr, m_depthTarget.m_DepthStencilView.Get());
			break;
		}

		m_d3d->GetContext()->ClearDepthStencilView(m_depthTarget.m_DepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0u);
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
		ComPtr<ID3D11Texture2D> backBuffer = nullptr;
		HRESULT hr = E_FAIL;
		
		
		// Get back buffer texture description
		PRX_ASSERT_HR(hr = m_d3d->GetSwapChain()->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.ReleaseAndGetAddressOf())),
			"Failed to get back buffer");
		PRX_FAIL_HR(hr);

		CREATE_ZERO(D3D11_TEXTURE2D_DESC, texDesc);
		backBuffer->GetDesc(&texDesc);

		// Create shader resource view description fro render target
		CREATE_ZERO(D3D11_SHADER_RESOURCE_VIEW_DESC, rtSrvDesc);
		rtSrvDesc.Format                    = texDesc.Format;
		rtSrvDesc.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2DMS;
		rtSrvDesc.Texture2D.MipLevels       = 1;
		rtSrvDesc.Texture2D.MostDetailedMip = 0;
		
		// Init render target texture
		m_backBuffer.m_Texture.m_width  = texDesc.Width;
		m_backBuffer.m_Texture.m_height = texDesc.Height;
		m_backBuffer.m_Texture.m_Tex2D  = backBuffer;
		m_backBuffer.m_IsBackBuffer     = true;
		m_backBuffer.m_Texture.m_name   = "Back Buffer Render Target";

		// Create render target srv
		PRX_ASSERT_HR(hr = m_d3d->GetDevice()->CreateShaderResourceView(backBuffer.Get(), &rtSrvDesc, m_backBuffer.m_Texture.m_SRV.ReleaseAndGetAddressOf()),
			"Failed to create shader resource view from back buffer");
		PRX_FAIL_HR(hr);

		// Create render target
		PRX_ASSERT_HR(hr = m_d3d->GetDevice()->CreateRenderTargetView(backBuffer.Get(), nullptr, m_backBuffer.m_RenderTargetView.ReleaseAndGetAddressOf()),
			"Failed to create back buffer render target view")
		PRX_FAIL_HR(hr);



		// Create frame buffer
		CREATE_ZERO(D3D11_TEXTURE2D_DESC, fbTexDesc);
		fbTexDesc.Width          = m_backBuffer.m_Texture.m_width;
		fbTexDesc.Height         = m_backBuffer.m_Texture.m_height;
		fbTexDesc.ArraySize      = 1;
		fbTexDesc.MipLevels      = 1;
		fbTexDesc.Format         = texDesc.Format;
		fbTexDesc.Usage          = D3D11_USAGE_DEFAULT;
		fbTexDesc.BindFlags      = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		fbTexDesc.CPUAccessFlags = 0;
		fbTexDesc.SampleDesc.Count = 1;
		fbTexDesc.MiscFlags      = 0;
		PRX_ASSERT_HR(hr = m_d3d->GetDevice()->CreateTexture2D(&fbTexDesc, NULL, m_frameBuffer.m_Texture.m_Tex2D.ReleaseAndGetAddressOf()),
			"Failed to create Texture2D for frame buffer");
		PRX_FAIL_HR(hr);

		m_frameBuffer.m_Texture.m_width  = m_backBuffer.m_Texture.m_width;
		m_frameBuffer.m_Texture.m_height = m_backBuffer.m_Texture.m_height;
		m_frameBuffer.m_IsBackBuffer     = false;
		m_frameBuffer.m_Texture.m_name   = "Frame Buffer Render Target";

		PRX_ASSERT_HR(hr = m_d3d->GetDevice()->CreateShaderResourceView(m_frameBuffer.m_Texture.m_Tex2D.Get(), &rtSrvDesc, m_frameBuffer.m_Texture.m_SRV.ReleaseAndGetAddressOf()),
			"Failed to create shader resource view from back buffer");
		PRX_FAIL_HR(hr);

		CREATE_ZERO(D3D11_RENDER_TARGET_VIEW_DESC, fbRtvDesc);
		fbRtvDesc.Format             = texDesc.Format;
		fbRtvDesc.ViewDimension      = D3D11_RTV_DIMENSION_TEXTURE2D;
		fbRtvDesc.Texture2D.MipSlice = 0;
		PRX_ASSERT_HR(hr = m_d3d->GetDevice()->CreateRenderTargetView(m_frameBuffer.m_Texture.m_Tex2D.Get(), &fbRtvDesc, m_frameBuffer.m_RenderTargetView.ReleaseAndGetAddressOf()),
			"Failed to create back buffer render target view");
		PRX_FAIL_HR(hr);





		
		// Create depth buffer texture
		CREATE_ZERO(D3D11_TEXTURE2D_DESC, dbDesc);
		dbDesc.Width              = texDesc.Width;
		dbDesc.Height             = texDesc.Height;
		dbDesc.MipLevels          = 1;
		dbDesc.ArraySize          = 1;
		dbDesc.Format             = DXGI_FORMAT_R32_TYPELESS;
		dbDesc.SampleDesc.Count   = texDesc.SampleDesc.Count;
		dbDesc.SampleDesc.Quality = texDesc.SampleDesc.Quality;
		dbDesc.Usage              = D3D11_USAGE_DEFAULT;
		dbDesc.BindFlags          = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		dbDesc.CPUAccessFlags     = 0;
		dbDesc.MiscFlags          = 0;
		hr = m_d3d->GetDevice()->CreateTexture2D(&dbDesc, NULL, m_depthTarget.m_Texture.m_Tex2D.ReleaseAndGetAddressOf());
		PRX_FAIL_HR(hr);

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


		// Create shader resource view description fro render target
		CREATE_ZERO(D3D11_DEPTH_STENCIL_VIEW_DESC, dsvDesc);
		dsvDesc.Format             = DXGI_FORMAT_D32_FLOAT;
		dsvDesc.ViewDimension      = D3D11_DSV_DIMENSION_TEXTURE2DMS;
		dsvDesc.Texture2D.MipSlice = 0;

		hr = m_d3d->GetDevice()->CreateDepthStencilView(m_depthTarget.m_Texture.m_Tex2D.Get(), &dsvDesc, m_depthTarget.m_DepthStencilView.ReleaseAndGetAddressOf());
		PRX_FAIL_HR(hr);

		// Create shader resource view description for depth target
		CREATE_ZERO(D3D11_SHADER_RESOURCE_VIEW_DESC, dbSrvDesc);
		dbSrvDesc.Format                    = DXGI_FORMAT_R32_FLOAT;
		dbSrvDesc.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2DMS;
		dbSrvDesc.Texture2D.MipLevels       = -1;
		dbSrvDesc.Texture2D.MostDetailedMip = 0;
		
		// Create shader resource view from depth texture
		hr = m_d3d->GetDevice()->CreateShaderResourceView(m_depthTarget.m_Texture.m_Tex2D.Get(), &dbSrvDesc, m_depthTarget.m_Texture.m_SRV.ReleaseAndGetAddressOf());
		PRX_FAIL_HR(hr);

		m_depthTarget.m_Texture.m_width = texDesc.Width;
		m_depthTarget.m_Texture.m_height = texDesc.Height;
		m_depthTarget.m_Texture.m_name = "Back Buffer Depth Target";

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