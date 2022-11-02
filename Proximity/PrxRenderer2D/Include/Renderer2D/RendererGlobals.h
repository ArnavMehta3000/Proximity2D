#pragma once
#include "../../PrxEngine/Include/PrxMacros.h"
#include <d3d11.h>
#include <wrl.h>

namespace Proximity::Graphics
{
	ComPtr<ID3D11Device>        g_device;
	ComPtr<ID3D11DeviceContext> g_context;
	ComPtr<IDXGISwapChain>      g_swapChain;
	ComPtr<ID3D11DeviceContext> g_defferedContext;

	float g_rendererWidth, g_rendererHeight;
	HWND g_hWnd;
}