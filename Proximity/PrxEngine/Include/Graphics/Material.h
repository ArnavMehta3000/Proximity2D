#pragma once
#include "Graphics/GPUShader.h"

namespace Proximity::Graphics
{
	enum class GPUShaderVarType
	{
		Unknown,
		BOOL,
		INT, INT2, INT3, INT4,
		UINT, UINT2, UINT3, UINT4,
		FLOAT, FLOAT2, FLOAT3, FLOAT4
	};


	using ShaderVar_T = std::variant<
		bool,
		int,   DirectX::XMINT2,   DirectX::XMINT3,   DirectX::XMINT4,
		UINT,  DirectX::XMUINT2,  DirectX::XMUINT3,  DirectX::XMUINT4,
		float, DirectX::XMFLOAT2, DirectX::XMFLOAT3, DirectX::XMFLOAT4,
		void*>;

	struct GPUShaderVariable
	{
		friend class Material;

		GPUShaderVariable()
			:
			Type(GPUShaderVarType::Unknown),
			Size(0),
			Offset(0)
		{}
		
		GPUShaderVariable(GPUShaderVarType type, void* data, size_t size, size_t offset)
			:
			Type(type),
			Size(size),
			Offset(offset)
		{}

		void SetIf(ShaderVar_T value)
		{
			Data = value;
		}

		// Templated function to get variant pointer to data
		template <typename T>
		auto GetIf() const noexcept
		{
			return std::get_if<T>(&Data);
		}
		std::string      Name;
		GPUShaderVarType Type;
		size_t           Size;
		size_t           Offset;

	private:
		ShaderVar_T      Data;
	};


	struct GPUShaderConstantBuffer
	{
		GPUShaderConstantBuffer()
			:
			Buffer(nullptr),
			Desc()
		{}

		void Release()
		{
			COM_RELEASE(Buffer);
			Variables.clear();
		}

		ComPtr<ID3D11Buffer>     Buffer;
		D3D11_SHADER_BUFFER_DESC Desc;
		std::vector<GPUShaderVariable> Variables;  // This vector acts like the structure of data
	};

	class Material
	{
	public:
		Material() = default;
		Material(std::shared_ptr<Graphics::GPUShader> pixelShader, std::string_view materialName = "New Material");

		const std::string&                          GetName()                const noexcept { return m_materialName; }
		Math::U64                                   GetConstantBufferCount() const noexcept { return m_constantBuffers.size(); }
		const std::vector<GPUShaderConstantBuffer>& GetConstantBufferList()  const noexcept { return m_constantBuffers; }


		void Release();

	private:
		bool CreateCBReflection();

	private:
		std::string                          m_materialName;
		std::shared_ptr<Graphics::GPUShader> m_pixelShader;
		std::vector<GPUShaderConstantBuffer> m_constantBuffers;
	};
}