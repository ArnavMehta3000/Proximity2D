#pragma once
#include "Renderer2D/RendererGlobals.h"

namespace Proximity::Graphics
{
	class Renderer
	{
	public:
		static bool Init(HWND window, F32 width, F32 height, bool isVsync);
		static void Shutdown();

	private:
		static bool CoreInit(bool isVsync);
		static bool CreateRenderTarget();

	private:
		static ComPtr<ID3D11RenderTargetView> m_renderTargetView;

		static U32 s_vramSize;
		static U32 s_ramSize;
	};
}