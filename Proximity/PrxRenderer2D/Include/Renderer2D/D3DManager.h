#pragma once

namespace Proximity::Graphics
{
	class D3DManager
	{
		friend class Renderer2D;

	public:
		D3DManager();
		~D3DManager();

		bool Init(U32 width, U32 height, const bool isVsync, HWND window, DXGI_FORMAT frameBufferFormat);
		void Shutdown();

		void Present();

		F32  AspectRatio()  const noexcept { return static_cast<F32>(m_width) / static_cast<F32>(m_height); }
		U32  WindowWidth()  const noexcept { return m_width; }
		U32  WindowHeight() const noexcept { return m_height; }
		HWND WindowHandle() const noexcept { return m_hWnd; }

	private:
		ComPtr<IDXGISwapChain>      m_swapChain;
		ComPtr<ID3D11Device>        m_device;
		ComPtr<ID3D11DeviceContext> m_context;

		bool                        m_vsyncEnabled;
		HWND                        m_hWnd;

		U32                         m_width;
		U32                         m_height;
	};
}