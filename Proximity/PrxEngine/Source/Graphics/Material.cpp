#include "enginepch.h"
#include "Graphics/Material.h"

namespace Proximity::Graphics
{
	Material::Material(std::shared_ptr<Graphics::GPUShader>pixelShader, std::string_view materialName)
		:
		m_materialName(materialName),
		m_pixelShader(pixelShader)
	{
		PRX_ASSERT_MSG(pixelShader->GetReflection().Type == GPUShaderType::Pixel, "Shader type is not pixel shader");
		PRX_LOG_INFO("Created material from PS [%s]", pixelShader->GetName().c_str());
	}

	void Material::Release()
	{
	}
}