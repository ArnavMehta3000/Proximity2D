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
		void Resize();

		ID3D11Device1*        GetDevice()    const noexcept { return m_device.Get(); }
		ID3D11DeviceContext1* GetContext()   const noexcept { return m_context.Get(); }
		IDXGISwapChain1*      GetSwapChain() const noexcept { return m_swapChain.Get(); }


		Math::F32  AspectRatio()  const noexcept { return static_cast<Math::F32>(m_width) / static_cast<Math::F32>(m_height); }
		Math::U32  WindowWidth()  const noexcept { return m_width; }
		Math::U32  WindowHeight() const noexcept { return m_height; }
		HWND WindowHandle() const noexcept { return m_hWnd; }

	private:
		ComPtr<IDXGISwapChain1>      m_swapChain;
		ComPtr<ID3D11Device1>        m_device;
		ComPtr<ID3D11DeviceContext1> m_context;

		bool                        m_vsyncEnabled;
		HWND                        m_hWnd;

		Math::U32                   m_width;
		Math::U32                   m_height;
	};
}