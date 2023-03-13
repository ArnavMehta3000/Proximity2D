#pragma once
namespace Proximity::Graphics
{
	class D3DManager : public Utils::IService
	{
	public:
		D3DManager();
		~D3DManager();

		bool Init(Math::U32 width, Math::U32 height, const bool isVsync, HWND window, DXGI_FORMAT frameBufferFormat);
		void Shutdown();

		void Present();
		void SetViewport(Math::U32 width, Math::U32 height);

		ID3D11Device*        GetDevice()    const noexcept { return m_device.Get(); }
		ID3D11DeviceContext* GetContext()   const noexcept { return m_context.Get(); }
		IDXGISwapChain*      GetSwapChain() const noexcept { return m_swapChain.Get(); }


		Math::F32   AspectRatio()     const noexcept { return static_cast<Math::F32>(m_width) / static_cast<Math::F32>(m_height); }
		Math::U32   WindowWidth()     const noexcept { return m_width; }
		Math::U32   WindowHeight()    const noexcept { return m_height; }
		HWND        WindowHandle()    const noexcept { return m_hWnd; }
		DXGI_FORMAT SwapChainFormat() const noexcept { return m_swapChainFormat; }

	private:
		ComPtr<IDXGISwapChain>      m_swapChain;
		ComPtr<ID3D11Device>        m_device;
		ComPtr<ID3D11DeviceContext> m_context;

		DXGI_FORMAT                 m_swapChainFormat;
		bool                        m_vsyncEnabled;
		HWND                        m_hWnd;

		Math::U32                   m_width;
		Math::U32                   m_height;
	};
}