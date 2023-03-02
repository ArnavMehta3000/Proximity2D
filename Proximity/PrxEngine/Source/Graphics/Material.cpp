#include "enginepch.h"
#include "Graphics/D3DManager.h"
#include "Graphics/Material.h"

#define SET_SHADER_VAR_DEF(varName, enumType, castType)\
varName.Type = enumType; \
varName.m_data = *static_cast<castType>(varDesc.DefaultValue); \
varName.m_dataDefault = *static_cast<castType>(varDesc.DefaultValue)

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
			buffer.Slot = i;
			
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
				
				// Ignore matrices
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
						variable.m_data = varDesc.DefaultValue;
						return false;
					}
					// Push  variable into buffer if not matrix
					buffer.Variables.push_back(variable);
				}
			}

			// Create buffer object
			CREATE_ZERO(D3D11_BUFFER_DESC, cbDesc);
			cbDesc.Usage               = D3D11_USAGE_DYNAMIC;
			cbDesc.BindFlags           = D3D11_BIND_CONSTANT_BUFFER;
			cbDesc.CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE;
			cbDesc.MiscFlags           = 0;
			cbDesc.ByteWidth           = buffer.Desc.Size;
			cbDesc.StructureByteStride = 0;
			HRESULT hr                 = Core::Globals::g_engineServices.ResolveService<Graphics::D3DManager>()->GetDevice()->CreateBuffer(&cbDesc, NULL, buffer.Buffer.ReleaseAndGetAddressOf());
			PRX_FAIL_HR(hr);
			buffer.ApplyBufferChanges();
			m_constantBuffers.push_back(buffer);
		}

		auto context = Core::Globals::g_engineServices.ResolveService<Graphics::D3DManager>()->GetContext();
		context->PSSetConstantBuffers(0, 1, m_constantBuffers[0].Buffer.GetAddressOf());

		

		return true;
	}


	void GPUShaderConstantBuffer::ApplyBufferChanges() const
	{
		CREATE_ZERO(D3D11_MAPPED_SUBRESOURCE, map);
		auto context = Core::Globals::g_engineServices.ResolveService<Graphics::D3DManager>()->GetContext();
		HRESULT hr   = context->Map(Buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
		
		if (FAILED(hr))
			PRX_LOG_ERROR("Failed to apply constant buffer changes [Buffer: %s]", Desc.Name);

		for (auto& var : Variables)
		{
			char* startPos = static_cast<char*>(map.pData) + var.Offset;
			
			auto value = var.GetValueByType();
			memcpy(startPos, value, var.Size);
		}

		context->Unmap(Buffer.Get(), 0);
	}

	const void* GPUShaderVariable::GetValueByType() const
	{
		using shVarType = Graphics::GPUShaderVarType;
		switch (Type)
		{
		case shVarType::BOOL:
			return GetIf<bool>();

		case shVarType::INT:
			return GetIf<int>();

		case shVarType::INT2:
			return GetIf<DirectX::XMINT2>();

		case shVarType::INT3:
			return GetIf<DirectX::XMINT3>();

		case shVarType::INT4:
			return GetIf<DirectX::XMINT4>();


		case shVarType::UINT:
			return GetIf<UINT>();

		case shVarType::UINT2:
			return GetIf<DirectX::XMUINT2>();

		case shVarType::UINT3:
			return GetIf<DirectX::XMUINT3>();

		case shVarType::UINT4:
			return GetIf<DirectX::XMUINT4>();


		case shVarType::FLOAT:
			return GetIf<float>();

		case shVarType::FLOAT2:
			return GetIf<DirectX::XMFLOAT2>();

		case shVarType::FLOAT3:
			return GetIf<DirectX::XMFLOAT3>();

		case shVarType::FLOAT4:
			return GetIf<DirectX::XMFLOAT4>();

		case shVarType::Unknown:
		default:
			return nullptr;
		}
	}
}