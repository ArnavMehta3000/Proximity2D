#pragma once
#include "Graphics/GPUShader.h"
#include "Graphics/Rendering/RenderingStructures.h"


namespace Proximity::Graphics
{
	enum class MaterialVarType
	{
		Unknown,
		BOOL,
		INT, INT2, INT3, INT4,
		UINT, UINT2, UINT3, UINT4,
		FLOAT, FLOAT2, FLOAT3, FLOAT4
	};


	typedef std::variant<
		bool,
		int,   DirectX::XMINT2,   DirectX::XMINT3,   DirectX::XMINT4,
		UINT,  DirectX::XMUINT2,  DirectX::XMUINT3,  DirectX::XMUINT4,
		float, DirectX::XMFLOAT2, DirectX::XMFLOAT3, DirectX::XMFLOAT4,
		void*> 
	ShaderVar_T;

	typedef std::pair<std::shared_ptr<Graphics::GPUShader>, std::shared_ptr<Graphics::GPUShader>> ShaderPair;  // Contains the 2 shaders bound for this material
	typedef std::variant<std::shared_ptr<Graphics::Texture2D>, SamplerState> ShaderInputVar_T;  // Contains the supported shader input types

	struct MaterialInputResource
	{
		GPUShaderType			 ShaderType;
		std::string              Name;
		D3D_SHADER_INPUT_TYPE    Type;
		UINT                     BindPoint;
		UINT                     BindCount;
		D3D_RESOURCE_RETURN_TYPE ReturnType;
		mutable ShaderInputVar_T         Resource;

		// Templated function to get variant pointer to data
		template <typename T>
		auto GetIf() const noexcept
		{
			return std::get_if<T>(&Resource);
		}

		void SetData(const ShaderInputVar_T& val) const
		{
			Resource = val;
		}

		void Bind() const ;
	};


	struct MaterialVariable
	{
		friend class Material;
		friend struct MaterialConstantBuffer;

		MaterialVariable()
			:
			Type(MaterialVarType::Unknown),
			Size(0),
			Offset(0)
		{}

		// Templated function to get variant pointer to data
		template <typename T>
		auto GetIf() const noexcept
		{
			return std::get_if<T>(&m_data);
		}

		// Set internal variant pointer data value
		void SetData(const ShaderVar_T& val) const
		{
			m_data = val;
		}

		void SetDefaultValue() const
		{
			m_data = m_dataDefault;
		}

		const void* GetValueByType() const;

		std::string      Name;
		MaterialVarType  Type;
		size_t           Size;
		size_t           Offset;
		
	private:
		mutable ShaderVar_T      m_data;
		ShaderVar_T              m_dataDefault;
	};


	struct MaterialConstantBuffer
	{
		MaterialConstantBuffer()
			:
			Buffer(nullptr),
			Desc(),
			Slot(0),
			Type(GPUShaderType::None)
		{}

		void Release()
		{
			COM_RELEASE(Buffer);
			Variables.clear();
		}

		void Bind()const noexcept;

		void ApplyBufferChanges() const noexcept;
		void ResetBufferValues() const noexcept
		{
			std::for_each(Variables.begin(), Variables.end(), [](const MaterialVariable& var)
			{
				var.SetDefaultValue();
			});
		}

		ComPtr<ID3D11Buffer>           Buffer;
		Math::U32                      Slot;
		GPUShaderType				   Type;
		D3D11_SHADER_BUFFER_DESC       Desc;
		std::vector<MaterialVariable>  Variables;  // This vector acts like the structure of data
	};

	class Material
	{		
	public:
		Material() = default;
		Material(std::shared_ptr<Graphics::GPUShader> shader1, std::shared_ptr<Graphics::GPUShader> shader2, std::string_view materialName = "New Material");

		const ShaderPair GetShaderPair() { return { m_vertexShader, m_pixelShader }; }
		
		bool SetBufferVarByName(const std::string_view& bufferName, const std::string_view& varName, const ShaderVar_T& value);

		// Set the vertex and pixel shaders
		const void Apply() const noexcept;

		const std::string&                         GetName()                const noexcept { return m_materialName; }
		Math::U64                                  GetConstantBufferCount() const noexcept { return m_constantBuffers.size(); }
		Math::U64                                  GetInputResourceCount()  const noexcept { return m_inputResources.size(); }
		const std::vector<MaterialConstantBuffer>& GetConstantBufferList()  const noexcept { return m_constantBuffers; }
		const std::vector<MaterialInputResource>&  GetInputResourceList()   const noexcept { return m_inputResources; }

		void Release();

		GPUShaderCompileInfo ReflectInputSlotByName(LPCSTR name, const std::string& shader);

	private:
		bool CreateCBReflection(const std::shared_ptr<Graphics::GPUShader>& shader, GPUShaderType type);

	private:
		std::string                          m_materialName;
		std::shared_ptr<Graphics::GPUShader> m_vertexShader;
		std::shared_ptr<Graphics::GPUShader> m_pixelShader;
		std::vector<MaterialConstantBuffer>  m_constantBuffers;
		std::vector<MaterialInputResource>   m_inputResources;
	};
}