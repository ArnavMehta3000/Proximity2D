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
		Material() = default;
		Material(std::shared_ptr<Graphics::GPUShader> pixelShader, std::string_view materialName = "New Material");

		const std::string& GetName() const noexcept { return m_materialName; }

		void Release();
	private:
		std::string m_materialName;
		std::shared_ptr<Graphics::GPUShader> m_pixelShader;
	};
}