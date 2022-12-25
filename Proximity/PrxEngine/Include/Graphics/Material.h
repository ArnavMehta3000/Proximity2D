#pragma once
#include "Graphics/GPUShader.h"

namespace Proximity::Graphics
{
	struct MaterialShaderEntries
	{
		std::string_view VertexEntry = "VSMain";
		std::string_view PixelEntry = "PSMain";
	};

	class Material
	{
	public:
		Material();
	private:
	};
}