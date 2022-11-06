#include "enginepch.h"
#include "Graphics/Texture2D.h"

namespace Proximity::Graphics
{
	Texture2DDesc::Texture2DDesc()
		:
		Width(1),
		Height(1),
		Format(DXGI_FORMAT_R32G32B32A32_FLOAT),
		BindFlags(D3D11_BIND_SHADER_RESOURCE),
		Data(nullptr),
		DataSlicePitch(1),
		ArraySize(1),
		CPUAccess(0u)
	{
	}
}