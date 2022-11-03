#pragma once

#define WIN32_LEAN_AND_MEAN
// Windows includes
#include <Windows.h>
#include <memory>
#include <functional>
#include <algorithm>

// Containers
#include <string>
#include <vector>
#include <array>
#include <queue>
#include <tuple>

// DX11 includes
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
#include <d3dcompiler.h>
#include <d3d11.h>
#include <wrl.h>

// Library links
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "D3DCompiler.lib")

namespace DX = DirectX;
using Microsoft::WRL::ComPtr;

// Renderer includes
#include <PrxUtils.h>
#include <PrxMath.h>

using namespace Proximity::Math;
namespace PrxUtils = Proximity::Utils;