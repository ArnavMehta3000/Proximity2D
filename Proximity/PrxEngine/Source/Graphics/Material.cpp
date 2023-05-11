#include "enginepch.h"
#include "Graphics/D3DManager.h"
#include "Graphics/Material.h"
#include "Engine/Modules/TextureLibrary.h"
#include "Graphics/Rendering/Renderer2D.h"
#include "optick/include/optick.h"

#define SET_SHADER_VAR_DEF(varName, enumType, castType, defaultVal)\
varName.Type = enumType; \
varName.m_data = *static_cast<castType>(defaultVal); \
varName.m_dataDefault = *static_cast<castType>(defaultVal)

namespace Proximity::Graphics
{
	Material::Material(std::shared_ptr<Graphics::GPUShader> shader1, std::shared_ptr<Graphics::GPUShader> shader2, std::string_view materialName)
		:
		m_materialName(materialName),
		m_vertexShader(shader1),
		m_pixelShader(shader2)
	{
		OPTICK_EVENT("Material::Material")
		if (!CreateCBReflection(m_vertexShader, GPUShaderType::Vertex))
			PRX_LOG_ERROR("Failed to reflect material constant buffer (VS) for [%s]", m_vertexShader->GetName().c_str());

		if (!CreateCBReflection(m_pixelShader, GPUShaderType::Pixel))
			PRX_LOG_ERROR("Failed to reflect material constant buffer (PS) for [%s]", m_pixelShader->GetName().c_str());

		// Reflect default input slots
		if (m_pixelShader->GetName() == "Internal PS")
		{
			ReflectInputSlotByName("tex", m_pixelShader->GetName());
			ReflectInputSlotByName("textureSampler", m_pixelShader->GetName());
		}

		PRX_LOG_INFO("Created material from shaders [%s] & [%s]", shader1->GetName().c_str(), shader2->GetName().c_str());
	}

	bool Material::SetBufferVarByName(const std::string_view& bufferName, const std::string_view& varName, const ShaderVar_T& value)
	{
		for (auto& cb : m_constantBuffers)
		{
			if (std::string(cb.Desc.Name).compare(bufferName) == 0)
			{
				for (auto& var : cb.Variables)
				{
					if (var.Name.compare(varName) == 0)
					{
						var.SetData(value);
						return true;
					}
				}
			}
		}

		return false;
	}

	bool Material::SetInputResourceByName(const std::string& resourceName, const ShaderInputVar_T value)
	{
		auto it = std::find_if(m_inputResources.begin(), m_inputResources.end(),
			[&resourceName](Graphics::MaterialInputResource& res) { return (res.Name.compare(resourceName) == 0); });

		if (it == std::end(m_inputResources))
			return false;
		else
		{
			(*it).SetData(value);
			return true;
		}
	}

	const void Material::Apply() const noexcept
	{
		m_vertexShader->Bind();
		m_pixelShader->Bind();
		
		// Bind constant buffers
		for (auto& cb : m_constantBuffers)
			cb.Bind();

		// Bind textures/samplers
		for (auto& res : m_inputResources)
			res.Bind();
	}

	void Material::HotReload() noexcept
	{
		if (!CreateCBReflection(m_vertexShader, GPUShaderType::Vertex))
			PRX_LOG_ERROR("Failed to reflect material constant buffer (VS) for [%s]", m_vertexShader->GetName().c_str());

		if (!CreateCBReflection(m_pixelShader, GPUShaderType::Pixel))
			PRX_LOG_ERROR("Failed to reflect material constant buffer (PS) for [%s]", m_pixelShader->GetName().c_str());
	}

	void Material::Release()
	{
		if (m_constantBuffers.size() == 0)
			return;

		std::for_each(m_constantBuffers.begin(), m_constantBuffers.end(), [](MaterialConstantBuffer& cb) {cb.Release(); });

		m_constantBuffers.clear();
		m_inputResources.clear();
	}

	bool Material::CreateCBReflection(const std::shared_ptr<Graphics::GPUShader>& shader, GPUShaderType type)
	{
		OPTICK_EVENT("Material::CreateCBReflection")
		auto& reflector   = shader->GetReflector();
		Math::U32 cbCount = shader->GetReflection().ConstantBuffersCount;
		
		// Leave if there is no CB to reflect
		if (cbCount == 0)
		{
			PRX_LOG_INFO("No constant buffer to refelct for material: [%s]", m_materialName.c_str());
			return true;
		}
		else
		{
			for (auto& cBuffer : m_constantBuffers)
			{
				cBuffer.Variables.clear();
				cBuffer.Release();
			}
			m_constantBuffers.clear();
		}

		// Loop over every constant buffer
		for (Math::U32 i = 0; i < cbCount; i++)
		{
			MaterialConstantBuffer buffer = MaterialConstantBuffer();

			ID3D11ShaderReflectionConstantBuffer* cb = reflector->GetConstantBufferByIndex(i);
			cb->GetDesc(&buffer.Desc);
			buffer.Slot = i;
			buffer.Type = type;

			
			// Loop over all variables in constant buffer
			for (Math::U32 j = 0; j < buffer.Desc.Variables; j++)
			{
				ID3D11ShaderReflectionVariable* var = cb->GetVariableByIndex(j);
				CREATE_ZERO(D3D11_SHADER_VARIABLE_DESC, varDesc);
				var->GetDesc(&varDesc);

				auto type = var->GetType();
				CREATE_ZERO(D3D11_SHADER_TYPE_DESC, typeDesc);
				type->GetDesc(&typeDesc);


				MaterialVariable variable{};
				variable.Type   = MaterialVarType::Unknown;
				variable.Name   = std::string(varDesc.Name);
				variable.Size   = varDesc.Size;
				variable.Offset = varDesc.StartOffset;
				
				// Ignore matrices
				if (typeDesc.Class == D3D_SVC_MATRIX_ROWS || typeDesc.Class == D3D_SVC_MATRIX_COLUMNS)
					continue;

				LPVOID defValPtr = nullptr;
				switch (typeDesc.Type)
				{
				case D3D_SVT_BOOL:
					defValPtr = 0;
					SET_SHADER_VAR_DEF(variable, MaterialVarType::BOOL, bool*, defValPtr);
					break;

				case D3D_SVT_INT:
					switch (typeDesc.Columns)
					{
					case 1:
					{
						defValPtr = 0;
						SET_SHADER_VAR_DEF(variable, MaterialVarType::INT, int*, defValPtr);
						break;
					}

					case 2:
					{
						auto val = DirectX::XMINT2(0, 0);
						defValPtr = &val;
						SET_SHADER_VAR_DEF(variable, MaterialVarType::INT2, DirectX::XMINT2*, defValPtr);
						break;
					}

					case 3:
					{
						auto val = DirectX::XMINT3(0, 0, 0);
						defValPtr = &val;
						SET_SHADER_VAR_DEF(variable, MaterialVarType::INT3, DirectX::XMINT3*, defValPtr);
						break;
					}

					case 4:
					{
						auto val = DirectX::XMINT4(0, 0, 0, 0);
						defValPtr = &val;
						SET_SHADER_VAR_DEF(variable, MaterialVarType::INT4, DirectX::XMINT4*, defValPtr);
						break;
					}

					default:
						PRX_LOG_ERROR("Failed to parse variable class type [INT COLUMN COUNT ERROR]");
						break;
					}
					break;

				case D3D_SVT_UINT:
					switch (typeDesc.Columns)
					{
					case 1:
					{
						defValPtr = 0;
						SET_SHADER_VAR_DEF(variable, MaterialVarType::UINT, UINT*, defValPtr);
						break;
					}

					case 2:
					{
						auto val = DirectX::XMUINT2(0, 0);
						defValPtr = &val;
						SET_SHADER_VAR_DEF(variable, MaterialVarType::UINT2, DirectX::XMUINT2*, defValPtr);
						break;
					}

					case 3:
					{
						auto val = DirectX::XMUINT3(0, 0, 0);
						defValPtr = &val;
						SET_SHADER_VAR_DEF(variable, MaterialVarType::UINT3, DirectX::XMUINT3*, defValPtr);
						break;
					}

					case 4:
					{
						auto val = DirectX::XMUINT4(0, 0, 0, 0);
						defValPtr = &val;
						SET_SHADER_VAR_DEF(variable, MaterialVarType::UINT4, DirectX::XMUINT4*, defValPtr);
						break;
					}

					default:
						PRX_LOG_ERROR("Failed to parse variable class type [UINT COLUMN COUNT ERROR]");
						break;
					}
					break;

				case D3D_SVT_FLOAT:
					switch (typeDesc.Columns)
					{
					case 1:
					{
						defValPtr = 0;
						SET_SHADER_VAR_DEF(variable, MaterialVarType::FLOAT, float*, defValPtr);
						break;
					}

					case 2:
					{
						auto val = DirectX::XMFLOAT2(0.0f, 0.0f);
						defValPtr = &val;
						SET_SHADER_VAR_DEF(variable, MaterialVarType::FLOAT2, DirectX::XMFLOAT2*, defValPtr);
						break;
					}

					case 3:
					{
						auto val = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
						defValPtr = &val;
						SET_SHADER_VAR_DEF(variable, MaterialVarType::FLOAT3, DirectX::XMFLOAT3*, defValPtr);
						break;
					}

					case 4:
					{
						auto val = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
						defValPtr = &val;
						SET_SHADER_VAR_DEF(variable, MaterialVarType::FLOAT4, DirectX::XMFLOAT4*, defValPtr);
						break;
					}

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

			// Create buffer object
			CREATE_ZERO(D3D11_BUFFER_DESC, cbDesc);
			cbDesc.Usage               = D3D11_USAGE_DYNAMIC;
			cbDesc.BindFlags           = D3D11_BIND_CONSTANT_BUFFER;
			cbDesc.CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE;
			cbDesc.MiscFlags           = 0;
			cbDesc.ByteWidth           = buffer.Desc.Size;
			cbDesc.StructureByteStride = 0;
			HRESULT hr                 = PRX_RESOLVE(Graphics::D3DManager)->GetDevice()->CreateBuffer(&cbDesc, NULL, buffer.Buffer.ReleaseAndGetAddressOf());
			PRX_FAIL_HR(hr);
			buffer.ApplyBufferChanges();
			m_constantBuffers.push_back(buffer);
		}

		return true;
	}

	GPUShaderCompileInfo Material::ReflectInputSlotByName(LPCSTR name, const std::string& shader)
	{
		OPTICK_EVENT("Material::ReflectInputSlotByName")
		GPUShaderCompileInfo info{};
		// Check if reflected slot name already exists
		auto exists = std::find_if(m_inputResources.begin(), m_inputResources.end(),
			[name](MaterialInputResource& res)
			{
				return res.Name.compare(name) == 0;
			});

		// Exit if found in list
		if (exists != std::end(m_inputResources))
		{
			info.HResult = E_INVALIDARG;
			info.Message << "Requested slot has already been reflected!";
			return info;
		}

		// Check if requested slot name is a constant buffer
		auto foundAsCb = std::find_if(m_constantBuffers.begin(), m_constantBuffers.end(),
			[name](MaterialConstantBuffer& cb)
			{
				return strcmp(cb.Desc.Name, name) == 0;
			});

		// Exit if slot is a constant buffer
		if (foundAsCb != std::end(m_constantBuffers))
		{
			info.HResult = E_FAIL;
			info.Message << "Requested slot reflection [" << name << "] already exists as constant buffer at slot: " << (*foundAsCb).Slot << " in [" << shader << "]";
			return info;
		}

		

		ComPtr<ID3D11ShaderReflection>reflector;

		MaterialInputResource res{};

		if (m_vertexShader->GetName().compare(shader) == 0)
		{
			reflector = m_vertexShader->GetReflector();
			res.ShaderType = GPUShaderType::Vertex;
		}
		else if (m_pixelShader->GetName().compare(shader) == 0)
		{
			reflector = m_pixelShader->GetReflector();
			res.ShaderType = GPUShaderType::Pixel;
		}

		D3D11_SHADER_INPUT_BIND_DESC desc{};
		info.HResult = reflector->GetResourceBindingDescByName(name, &desc);

		if (FAILED(info.HResult))
		{
			info.Message << "Failed to reflect slot";
			return info;
		}
		else
			info.Message << "Found slot [" << name << "] in shader [" << shader << "]" << std::endl;

		// Check for texture/sampler slots
		bool found = false;
		switch (desc.Type)
		{
		case D3D_SIT_TEXTURE:
		{
			auto textureLib = PRX_RESOLVE(Modules::TextureLibrary);
			info.Message << "Shader input type is [Texture]";
			res.BindCount   = desc.BindCount;
			res.BindPoint   = desc.BindPoint;
			res.Type        = D3D_SIT_TEXTURE;
			res.Name        = desc.Name;
			found           = true;

			//res.Resource = textureLib->GetMap().at(0);  // 0 is always internal white texture
		}
			break;

		case D3D_SIT_SAMPLER:
		{
			auto renderer2D = PRX_RESOLVE(Graphics::Renderer2D);
			info.Message << "Shader input type is [Texture Sampler]";
			res.BindCount   = desc.BindCount;
			res.BindPoint   = desc.BindPoint;
			res.Type        = D3D_SIT_SAMPLER;
			res.Name        = desc.Name;
			found           = true;

			//res.Resource = renderer2D->GetSamplerList().at(0);
		}
			break;

		default:
			info.Message << "Reflection of input type not supported. Type [" << (int)desc.Type << "]";
			return info;
		}
		
		if (found)
			m_inputResources.push_back(res);

		return info;
	}




	void MaterialConstantBuffer::Bind() const noexcept
	{
		auto ctx = PRX_RESOLVE(Graphics::D3DManager)->GetContext();

		switch (Type)
		{
		case Proximity::Graphics::GPUShaderType::Vertex:
			ctx->VSSetConstantBuffers(Slot, 1, Buffer.GetAddressOf());
			break;
		case Proximity::Graphics::GPUShaderType::Pixel:
			ctx->PSSetConstantBuffers(Slot, 1, Buffer.GetAddressOf());
			break;
		default:
			PRX_LOG_ERROR("Failed to bind constant buffer");
			break;
		}
	}

	void MaterialConstantBuffer::ApplyBufferChanges() const noexcept
	{
		CREATE_ZERO(D3D11_MAPPED_SUBRESOURCE, map);
		auto context = PRX_RESOLVE(Graphics::D3DManager)->GetContext();
		HRESULT hr   = context->Map(Buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
		
		if (FAILED(hr))
			PRX_LOG_ERROR("Failed to apply constant buffer changes [Buffer: %s]", Desc.Name);

		for (auto& var : Variables)
		{
			char* startPos = static_cast<char*>(map.pData) + var.Offset;
			
			auto value = var.GetValue();
			memcpy(startPos, value, var.Size);
		}

		context->Unmap(Buffer.Get(), 0);
	}

	const void* MaterialVariable::GetValue() const
	{
		using shVarType = Graphics::MaterialVarType;
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

	void MaterialInputResource::Bind() const
	{
		auto ctx = PRX_RESOLVE(Graphics::D3DManager)->GetContext();


		switch (ShaderType)
		{
		case Proximity::Graphics::GPUShaderType::Vertex:
			switch (Type)
			{
			case D3D_SIT_TEXTURE:
			{
				auto vsTex = *GetIf<std::shared_ptr<Texture2D>>();
				if (vsTex == nullptr) return;
				ctx->VSSetShaderResources(BindPoint, 1, vsTex->SRV.GetAddressOf());
			}
				break;

			case D3D_SIT_SAMPLER:
			{
				auto vsSampler = GetIf<SamplerState>();
				if (vsSampler == nullptr) return;
				ctx->VSSetSamplers(BindPoint, 1, vsSampler->Sampler.GetAddressOf());
			}
				break;
			}
			break;
		case Proximity::Graphics::GPUShaderType::Pixel:
			switch (Type)
			{
			case D3D_SIT_TEXTURE:
			{
				auto psTex = *GetIf<std::shared_ptr<Texture2D>>();
				if (psTex == nullptr) return;
				ctx->PSSetShaderResources(BindPoint, 1, psTex->SRV.GetAddressOf());
			}
				break;

			case D3D_SIT_SAMPLER:
			{
				auto psSampler = GetIf<SamplerState>();
				if (psSampler == nullptr) return;
				ctx->PSSetSamplers(BindPoint, 1, psSampler->Sampler.GetAddressOf());
			}
				break;
			}
			break;
		}
	}
}