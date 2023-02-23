#include "enginepch.h"
#include "Graphics/Material.h"

namespace Proximity::Graphics
{
	Material::Material(const std::shared_ptr<Graphics::GPUShader>& pixelShader, std::string_view materialName)
		:
		m_materialName(materialName),
		m_pixelShader(pixelShader)
	{

	}

	void Material::Release()
	{
	}
}