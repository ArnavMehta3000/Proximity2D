#include "enginepch.h"
#include "Graphics/Material.h"

#define SET_SHADER_VAR_DEF(varName, enumType, castType)\
varName.Type = enumType; \
varName.Data = *static_cast<castType>(varDesc.DefaultValue)

namespace Proximity::Graphics
{
	Material::Material(std::shared_ptr<Graphics::GPUShader>pixelShader, std::string_view materialName)
		:
		m_materialName(materialName),
		m_pixelShader(pixelShader)
	{
		PRX_ASSERT_MSG(pixelShader->GetReflection().Type == GPUShaderType::Pixel, "Failed to create Material\nShader type is not pixel shader");

		if (!CreateCBReflection())
		{
			PRX_LOG_ERROR("Failed to reflect material constant buffer");
		}

		PRX_LOG_INFO("Created material from PS [%s]", pixelShader->GetName().c_str());
	}

	void Material::Release()
	{
		if (m_constantBuffers.size() == 0)
			return;

		std::for_each(m_constantBuffers.begin(), m_constantBuffers.end(), [](GPUShaderConstantBuffer& cb) {cb.Release(); });

		m_constantBuffers.clear();
	}



	bool Material::CreateCBReflection()
	{
		auto& reflector = m_pixelShader->GetReflector();
		Math::U32 cbCount = m_pixelShader->GetReflection().ConstantBuffersCount;
		
		// Leave if there is no CB to reflect
		if (cbCount == 0)
		{
			PRX_LOG_INFO("No constant buffer to refelct for material: [%s]", m_materialName.c_str());
			return true;
		}

		// Loop over every constant buffer
		for (Math::U32 i = 0; i < cbCount; i++)
		{
			GPUShaderConstantBuffer buffer = GPUShaderConstantBuffer();

			ID3D11ShaderReflectionConstantBuffer* cb = reflector->GetConstantBufferByIndex(i);
			cb->GetDesc(&buffer.Desc);

			
			// Loop over all variables in constant buffer
			for (Math::U32 j = 0; j < buffer.Desc.Variables; j++)
			{
				ID3D11ShaderReflectionVariable* var = cb->GetVariableByIndex(j);
				CREATE_ZERO(D3D11_SHADER_VARIABLE_DESC, varDesc);
				var->GetDesc(&varDesc);

				auto type = var->GetType();
				CREATE_ZERO(D3D11_SHADER_TYPE_DESC, typeDesc);
				type->GetDesc(&typeDesc);


				GPUShaderVariable variable{};
				variable.Type   = GPUShaderVarType::Unknown;
				variable.Name   = std::string(varDesc.Name);
				variable.Size   = varDesc.Size;
				variable.Offset = varDesc.StartOffset;
				
				if (!(typeDesc.Class == D3D_SVC_MATRIX_ROWS || typeDesc.Class == D3D_SVC_MATRIX_COLUMNS))
				{
					switch (typeDesc.Type)
					{
					case D3D_SVT_BOOL:
						SET_SHADER_VAR_DEF(variable, GPUShaderVarType::BOOL, bool*);
						break;

					case D3D_SVT_INT:
						switch (typeDesc.Columns)
						{
						case 1:
							SET_SHADER_VAR_DEF(variable, GPUShaderVarType::INT, int*);
							break;

						case 2:
							SET_SHADER_VAR_DEF(variable, GPUShaderVarType::INT2, DirectX::XMINT2*);
							break;

						case 3:
							SET_SHADER_VAR_DEF(variable, GPUShaderVarType::INT3, DirectX::XMINT3*);
							break;

						case 4:
							SET_SHADER_VAR_DEF(variable, GPUShaderVarType::INT4, DirectX::XMINT4*);
							break;

						default:
							PRX_LOG_ERROR("Failed to parse variable class type [INT COLUMN COUNT ERROR]");
							break;
						}
						break;

					case D3D_SVT_UINT:
						switch (typeDesc.Columns)
						{
						case 1:
							SET_SHADER_VAR_DEF(variable, GPUShaderVarType::UINT, UINT*);
							break;

						case 2:
							SET_SHADER_VAR_DEF(variable, GPUShaderVarType::UINT2, DirectX::XMUINT2*);
							break;

						case 3:
							SET_SHADER_VAR_DEF(variable, GPUShaderVarType::UINT3, DirectX::XMUINT3*);
							break;

						case 4:
							SET_SHADER_VAR_DEF(variable, GPUShaderVarType::UINT4, DirectX::XMUINT4*);
							break;

						default:
							PRX_LOG_ERROR("Failed to parse variable class type [UINT COLUMN COUNT ERROR]");
							break;
						}
						break;

					case D3D_SVT_FLOAT:
						switch (typeDesc.Columns)
						{
						case 1:
							SET_SHADER_VAR_DEF(variable, GPUShaderVarType::FLOAT, float*);
							break;

						case 2:
							SET_SHADER_VAR_DEF(variable, GPUShaderVarType::FLOAT2, DirectX::XMFLOAT2*);
							break;

						case 3:
							SET_SHADER_VAR_DEF(variable, GPUShaderVarType::FLOAT3, DirectX::XMFLOAT3*);
							break;

						case 4:
							SET_SHADER_VAR_DEF(variable, GPUShaderVarType::FLOAT4, DirectX::XMFLOAT4*);
							break;

						default:
							PRX_LOG_ERROR("Failed to parse variable class type [FLOAT COLUMN COUNT ERROR]");
							break;
						}
						break;

					default:
						PRX_LOG_ERROR("Failed to parse variable class type [TYPE NOT SUPPORTED]");
						variable.Data = varDesc.DefaultValue;
						return false;
					}
					// Push  variable into buffer if not matrix
					buffer.Variables.push_back(variable);
				}

			}
			m_constantBuffers.push_back(buffer);
		}


		return true;
	}
}