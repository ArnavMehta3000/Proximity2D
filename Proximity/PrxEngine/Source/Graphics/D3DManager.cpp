#include "enginepch.h"
#include "Graphics/D3DManager.h"

namespace Proximity::Graphics
{
	D3DManager::D3DManager()
		:
		m_swapChain(nullptr),
		m_device(nullptr),
		m_context(nullptr),
		m_width(0u),
		m_height(0u),
		m_hWnd(NULL),
		m_vsyncEnabled(false)
	{}

	D3DManager::~D3DManager()
	{}

	bool D3DManager::Init(U32 width, U32 height, const bool isVsync, HWND window, DXGI_FORMAT frameBufferFormat)
	{
		m_width           = width;
		m_height          = height;
		m_hWnd            = window;
		m_vsyncEnabled    = isVsync;
		m_swapChainFormat = frameBufferFormat;

		HRESULT hr = E_FAIL;
		ComPtr<IDXGIFactory> factory;
		ComPtr<IDXGIAdapter> adapter;
		ComPtr<IDXGIOutput> adapterOutput;
		U32 numModes;
		D3D_FEATURE_LEVEL featureLevel;

		PRX_ASSERT_HR(hr = CreateDXGIFactory(IID_PPV_ARGS(factory.ReleaseAndGetAddressOf())),
			"Failed to create IDXGIFactory");
		PRX_FAIL_HR(hr);
		PRX_LOG_DEBUG("DXFactory created");

		PRX_ASSERT_HR(hr = factory->EnumAdapters(0, adapter.ReleaseAndGetAddressOf()),
			"Failed to enumerate over factory adapters");
		PRX_FAIL_HR(hr);
		PRX_LOG_DEBUG("DXAdapter created");

		PRX_ASSERT_HR(hr = adapter->EnumOutputs(0, adapterOutput.ReleaseAndGetAddressOf()),
			"Failed to enumerate over adapter outputs");
		PRX_FAIL_HR(hr);
		PRX_LOG_DEBUG("DXAdapterOutput created");

		PRX_ASSERT_HR(hr = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL),
			"Failed to get display mode list count");
		PRX_FAIL_HR(hr);
		PRX_LOG_DEBUG("Display mode count: %u", numModes);

		DXGI_MODE_DESC* displayModeList;
		displayModeList = new DXGI_MODE_DESC[numModes];
		if (!displayModeList)
		{
			return false;
		}

		PRX_ASSERT_HR(hr = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList),
			"Failed to fill display mode list");
		PRX_FAIL_HR(hr);

		U32 numerator = 0, denominator = 0;
		for (U32 i = 0; i < numModes; i++)
		{
			auto mode = displayModeList[i];
			if (mode.Width = (U32)width)
				if (mode.Height = (U32)height)
				{
					numerator = mode.RefreshRate.Numerator;
					denominator = mode.RefreshRate.Denominator;
				}
		}
		PRX_LOG_DEBUG("Display numerator and denominator set: (%u, %u)", numerator, denominator);



		// Display not found
		if (numerator == 0 && denominator == 0)
		{
			auto mode = displayModeList[numModes / 2];
			numerator = mode.RefreshRate.Numerator;
			denominator = mode.RefreshRate.Denominator;

			width = mode.Width;
			height = mode.Height;

			PRX_LOG_ERROR("Specified resoltion [%ux%u] not found. Fallback to [%ux%u]", width, height, mode.Width, mode.Height);

			// Reposition window based
			SetWindowPos(window, 0, 10, 10, width, height, SWP_NOZORDER);
		}

		PRX_LOG_DEBUG("Display dimensions set: (%u, %u)", width, height);

		// Stop all alt-enter
		factory->MakeWindowAssociation(window, DXGI_MWA_NO_ALT_ENTER);


		// Get adpater description
		delete[] displayModeList;
		displayModeList = nullptr;

		COM_RELEASE(adapterOutput);
		COM_RELEASE(adapter);
		COM_RELEASE(factory);

		// Init swapchchain
		CREATE_ZERO(DXGI_SWAP_CHAIN_DESC, swapChainDesc);
		swapChainDesc.BufferCount                        = 2;
		swapChainDesc.BufferDesc.Width                   = width;
		swapChainDesc.BufferDesc.Height                  = height;
		swapChainDesc.BufferDesc.Format                  = frameBufferFormat;
		swapChainDesc.BufferDesc.RefreshRate.Numerator   = (isVsync) ? numerator : 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = (isVsync) ? denominator : 1;
		swapChainDesc.OutputWindow                       = window;
		swapChainDesc.Windowed                           = TRUE;  // application will not go fullscreen
		swapChainDesc.BufferDesc.ScanlineOrdering        = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling                 = DXGI_MODE_SCALING_UNSPECIFIED;
		swapChainDesc.SwapEffect                         = DXGI_SWAP_EFFECT_DISCARD;  // NOTE: Change this later
		swapChainDesc.BufferUsage                        = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;
		swapChainDesc.SampleDesc.Count                   = 1;
		swapChainDesc.SampleDesc.Quality                 = 0;
		swapChainDesc.Flags                              = 0;

		featureLevel = D3D_FEATURE_LEVEL_11_1;

		U32 creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
		creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif // _DEBUG


		PRX_ASSERT_HR(hr = D3D11CreateDeviceAndSwapChain(NULL,
														 D3D_DRIVER_TYPE_HARDWARE,
														 NULL,
														 creationFlags,
														 &featureLevel,
														 1,
														 D3D11_SDK_VERSION,
														 &swapChainDesc,
														 m_swapChain.ReleaseAndGetAddressOf(),
														 m_device.ReleaseAndGetAddressOf(),
														 NULL,
														 m_context.ReleaseAndGetAddressOf()),
			"Failed to create device and swap chain");
		PRX_FAIL_HR(hr);

		// Set viewport
		Resize(width, height);

		return true;
	}

	void D3DManager::Shutdown()
	{
		COM_RELEASE(m_swapChain);
		COM_RELEASE(m_context);
		COM_RELEASE(m_device);
	}

	void D3DManager::Present()
	{
		if (m_vsyncEnabled)
			m_swapChain->Present(1, 0);
		else
			m_swapChain->Present(0, 0);
	}

	void D3DManager::Resize(Math::U32 width, Math::U32 height)
	{
		D3D11_VIEWPORT vp;
		vp.Width    = static_cast<Math::F32>(width);
		vp.Height   = static_cast<Math::F32>(height);
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		m_context->RSSetViewports(1, &vp);
		PRX_LOG_DEBUG("D3D resize {%ux%u}", width, height);
	}
}