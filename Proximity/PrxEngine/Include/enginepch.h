#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
// Windows includes
#include <Windows.h>
#include <iostream>
#include <tchar.h>
#include <memory>
#include <typeinfo>
#include <type_traits>
#include <functional>
#include <algorithm>
#include <xhash>

// Containers and utils
#include <string>
#include <sstream>
#include <vector>
#include <array>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <queue>
#include <tuple>

// DX11 includes
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
#include <d3dcompiler.h>
#include <d3d11_4.h>
#include <wrl.h>

// Library links
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "D3DCompiler.lib")

namespace DX = DirectX;
using Microsoft::WRL::ComPtr;

// Engine includes
#include <PrxMath.h>
#include <PrxUtils.h>

#include "Engine/EngineGlobals.h"
#include "Utils/Assertions.h"
#include "Utils/Logger.h"
#include "Utils/Misc.h"

using namespace Proximity::Math;
using namespace Proximity::Utils;