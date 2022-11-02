#pragma once
#include <combaseapi.h>

namespace Proximity::Graphics
{
	class DeviceResources
	{
		struct ResCache
		{
			HWND WindowHandle;
			D3D_FEATURE_LEVEL D3DFeatureLevel;
			RECT OutputSize;
		};

		enum class ResourceDefaults
		{
			BackBufferFormat = DXGI_FORMAT_B8G8R8A8_UNORM,
			DepthBufferFormat = 
		};

	public:
		DeviceResources();
		~DeviceResources();

		void CreateDeviceResources();
		void CreateSizeDependentResource();
		void SetWindow() noexcept;
		bool Resize();
		void Present();

		auto              GetDevice()           const noexcept { return m_device.Get(); }
		auto              GetImContext()        const noexcept { return m_context.Get(); }
		auto              GetSwapChain()        const noexcept { return m_swapChain.Get(); }
		auto              GetRenderTarget()     const noexcept { return m_renderTarget.Get(); }
		auto              GetDepthStencil()     const noexcept { return m_depthStencil.Get(); }
		auto              GetRenderTargetView() const noexcept { return m_renderTargetView.Get(); }
		auto              GetDepthStencilView() const noexcept { return m_depthStencilView.Get(); }
		HWND              GetWindowHandle()     const noexcept { return m_cache.WindowHandle; }
		D3D_FEATURE_LEVEL GetD3DFeatureLevel()  const noexcept { return m_cache.D3DFeatureLevel; }


	private:
		void CreateFactory();
		void GetHardwareAdapter(IDXGIAdapter1** ppAdapter);

	private:
		ComPtr<ID3D11Device1>           m_device;
		ComPtr<ID3D11DeviceContext1>    m_context;
		ComPtr<IDXGISwapChain>          m_swapChain;

		ComPtr<ID3D11Texture2D>         m_renderTarget;
		ComPtr<ID3D11Texture2D>         m_depthStencil;
		ComPtr<ID3D11RenderTargetView>  m_renderTargetView;
		ComPtr<ID3D11DepthStencilView>  m_depthStencilView;

		D3D11_VIEWPORT m_viewport;

		ResCache m_cache;
	};
}
