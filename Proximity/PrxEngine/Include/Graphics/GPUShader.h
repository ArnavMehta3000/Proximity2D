#pragma once
#include <d3d11shader.h>

namespace Proximity::Graphics
{
	enum class GPUShaderType : int
	{
		None   = -1,
		Vertex = 0,
		Pixel  = 1
	};

	// Contains shader compilation information
	struct GPUShaderCompileInfo
	{
		HRESULT HResult;
		std::stringstream Message;
	};

	// Structure containing compiled shader reflection
	struct GPUShaderReflection
	{
		Math::U32     ConstantBuffersCount        = 0;
		Math::U32     Version                     = 0;
		Math::U32     BoundResources              = 0;
		Math::U32     InputParameters             = 0;
		Math::U32     TextureLoadInstructions     = 0;
		Math::U32     TextureCompInstructions     = 0;
		Math::U32     TextureBiasInstructions     = 0;
		Math::U32     TextureGradientInstructions = 0;
		Math::U32     TextureStoreInstructions    = 0;
		Math::U32     TextureNormalInstructions   = 0;
		Math::U32     FloatInstructionCount       = 0;
		Math::U32     IntInstructionCount         = 0;
		Math::U32     UintInstructionCount        = 0;
		Math::U32     StaticFlowControlCount      = 0;
		Math::U32     DynamicFlowControlCount     = 0;
		std::string   Creator                     = "";
		std::string   Name                        = "";
	};

	// Class containing the vertex and pixel shader. Supports compiling and hot reloading
	class GPUShader
	{
	public:
		static const std::string TypeToString(GPUShaderType type) noexcept;

		struct VertexShader
		{
			VertexShader()
				:
				Blob(nullptr),
				InputLayout(nullptr),
				Shader(nullptr)
			{}

			ComPtr<ID3DBlob> Blob;
			ComPtr<ID3D11InputLayout>  InputLayout;
			ComPtr<ID3D11VertexShader> Shader;
		};

		struct PixelShader
		{
			PixelShader()
				:
				Blob(nullptr),
				Shader(nullptr)
			{}

			ComPtr<ID3DBlob> Blob;
			ComPtr<ID3D11PixelShader> Shader;
		};

	public:
		GPUShader(std::string_view shaderName = "New Shader");

		static void CreateDefaults();


		const std::string GetTypeAsString()                  const noexcept { return TypeToString(m_shaderType); }
		const std::string&                   GetName()       const noexcept { return m_shaderName; }
		const GPUShaderType                  GetType()       const noexcept { return m_shaderType; }
		const GPUShaderReflection&           GetReflection() const noexcept { return m_reflection; }
		const ComPtr<ID3D11ShaderReflection> GetReflector()  const noexcept { return m_reflector; }
		bool IsInternal() const noexcept { return m_isInternal; }

		GPUShaderCompileInfo HotReload();
		void Bind();

		GPUShaderCompileInfo CompileShader(std::string_view path, std::string_view shaderEntry, GPUShaderType type);
		void Release();

	private:
		GPUShaderCompileInfo CompileVertexShader(ID3D11Device* device, std::string_view path, std::string_view shaderEntry);
		GPUShaderCompileInfo CompilePixelShader(ID3D11Device* device, std::string_view path, std::string_view shaderEntry);
		void CreateInputLayoutFromVS(ID3D11Device* device, GPUShaderCompileInfo& info);
		void CreateReflection();

	private:
		ComPtr<ID3D11ShaderReflection> m_reflector;
		GPUShaderReflection m_reflection;
		GPUShaderType       m_shaderType;
		VertexShader        m_vertexShader;
		PixelShader         m_pixelShader;

		std::string         m_shaderName;
		std::string         m_filePath;
		std::string         m_entrypoint;

		bool m_isInternal;
	};
}