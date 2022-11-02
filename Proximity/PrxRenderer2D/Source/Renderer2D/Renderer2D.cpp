#include "renderer2dpch.h"
#include "Renderer2D/Renderer2D.h"

namespace Proximity::Graphics
{
	bool Renderer::Init(HWND window, F32 width, F32 height, bool isVsync)
	{
		g_hWnd           = window;
		g_rendererWidth  = width;
		g_rendererHeight = height;

		return CoreInit(isVsync);
	}

	void Renderer::Shutdown()
	{
		COM_RELEASE(m_renderTargetView);
		COM_RELEASE(g_context);
		COM_RELEASE(g_defferedContext);
		COM_RELEASE(g_swapChain);
		COM_RELEASE(g_device);
	}

	bool Renderer::CoreInit(bool isVsync)
	{
		ComPtr<IDXGIFactory> factory;
		ComPtr<IDXGIAdapter> adapter;
		ComPtr<IDXGIOutput>  adapterOutput;

		DXGI_MODE_DESC* displayModeList;
		DXGI_ADAPTER_DESC adapterDesc;
		UINT numModes = 0, numerator = 0, denominator = 1;

		HRESULT hr = S_OK;

		PRX_ASSERT_HR(hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory),
			"Failed to create DXGI Factory");
		if (FAILED(hr))
			return false;
		
		PRX_ASSERT_HR(hr = factory->EnumAdapters(0, &adapter),
			"Faile to enumerate over factory adapters");
		if (FAILED(hr))
			return false;
		PRX_ASSERT_HR(hr = adapter->EnumOutputs(0, &adapterOutput),
			"Failed to enumerate over adapter outputs");
		if (FAILED(hr))
			return false;
		PRX_ASSERT_HR(hr = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL),
			"Failed to get display modes list");
		if (FAILED(hr))
			return false;

		displayModeList = new DXGI_MODE_DESC[numModes];
		PRX_ASSERT_HR(hr = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList),
			"Failed to get display modes");
		if (FAILED(hr))
			return false;

		for (unsigned int i = 0; i < numModes; i++)
		{
			if (displayModeList[i].Width == (unsigned int)g_rendererWidth)
			{
				if (displayModeList[i].Height == (unsigned int)g_rendererHeight)
				{
					numerator = displayModeList[i].RefreshRate.Numerator;
					denominator = displayModeList[i].RefreshRate.Denominator;
				}
			}
		}

		PRX_ASSERT_HR(hr = adapter->GetDesc(&adapterDesc),
			"Failed to get adapter description");
		if (FAILED(hr))
			return false;

		s_vramSize = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);
		s_ramSize  = (int)(adapterDesc.SharedSystemMemory / 1024 / 1024);

		delete[] displayModeList;
		displayModeList = 0;
		COM_RELEASE(adapter);
		COM_RELEASE(adapterOutput);


		// Create swapchain description
		CREATE_ZERO(DXGI_SWAP_CHAIN_DESC, swapChainDesc);
		swapChainDesc.BufferCount                            = 1;
		swapChainDesc.BufferDesc.Width                       = static_cast<U32>(g_rendererWidth);
		swapChainDesc.BufferDesc.Height                      = static_cast<U32>(g_rendererHeight);
		swapChainDesc.BufferDesc.Format                      = DXGI_FORMAT_R16G16B16A16_FLOAT;
		if (isVsync)
		{
			swapChainDesc.BufferDesc.RefreshRate.Numerator   = numerator;
			swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
		}
		else
		{
			swapChainDesc.BufferDesc.RefreshRate.Numerator   = 60;  // Lock swapchain refresh rate to 60hz
			swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		}
		swapChainDesc.BufferUsage                            = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;
		swapChainDesc.OutputWindow                           = g_hWnd;
		swapChainDesc.SampleDesc.Count                       = 1;  // Anti aliasing here
		swapChainDesc.SampleDesc.Quality                     = 0;
		swapChainDesc.Windowed                               = TRUE;
		swapChainDesc.BufferDesc.ScanlineOrdering            = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling                     = DXGI_MODE_SCALING_UNSPECIFIED;
		swapChainDesc.SwapEffect                             = DXGI_SWAP_EFFECT_DISCARD;
		swapChainDesc.Flags                                  = 0;

		
		auto driverType   = D3D_DRIVER_TYPE_HARDWARE;
		auto featureLevel = D3D_FEATURE_LEVEL_11_1;

		// Create device / swapchain / immediate context
		PRX_ASSERT_HR(hr = D3D11CreateDeviceAndSwapChain(
			nullptr,
			driverType,
			NULL,
			0,
			&featureLevel, 1,
			D3D11_SDK_VERSION,
			&swapChainDesc,
			g_swapChain.ReleaseAndGetAddressOf(),
			g_device.ReleaseAndGetAddressOf(),
			NULL,
			g_context.ReleaseAndGetAddressOf()),
		"Failed to create renderer device and swap chain");
		if (FAILED(hr))
			return false;

		factory->MakeWindowAssociation(g_hWnd, DXGI_MWA_NO_ALT_ENTER);
		COM_RELEASE(factory);

		PRX_LOG_INFO("Render initalized");

		return true;
	}

	bool Renderer::CreateRenderTarget()
	{
		HRESULT hr = S_OK;
		ComPtr<ID3D11Texture2D> backBufferPtr;

		PRX_ASSERT_HR(hr = g_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBufferPtr.ReleaseAndGetAddressOf())),
			"Failed to get back buffer");
		if (FAILED(hr))
			return false;

		PRX_ASSERT_HR(hr = g_device->CreateRenderTargetView(backBufferPtr.Get(), nullptr, m_renderTargetView.ReleaseAndGetAddressOf()),
			"Failed to create render target view");
		if (FAILED(hr))
			return false;

		COM_RELEASE(backBufferPtr);

		return true;
	}
}