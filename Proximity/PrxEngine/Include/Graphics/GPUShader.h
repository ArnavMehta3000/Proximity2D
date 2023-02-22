#pragma once
#include <d3d11shader.h>

namespace Proximity::Graphics
{
	enum class GPUShaderType
	{
		None = -1,
		Vertex = 0,
		Pixel
	};

	// Contains shader compilation information
	struct GPUShaderCompileInfo
	{
		HRESULT HResult;
		bool Succeeded;
		std::string Message;
	};

	// Structure containing compiled shader reflection
	struct GPUShaderReflection
	{
		GPUShaderType                         Type;
		Math::U32                             ConstantBuffersCount;
		Math::U32                             Version;
		Math::U32                             BoundResources;
		Math::U32                             InputParameters;
		Math::U32                             TextureLoadInstructions;
		Math::U32                             TextureCompInstructions;
		Math::U32                             TextureBiasInstructions;
		Math::U32                             TextureGradientInstructions;
		Math::U32                             TextureStoreInstructions;
		Math::U32                             TextureNormalInstructions;
		Math::U32                             FloatInstructionCount;
		Math::U32                             IntInstructionCount;
		Math::U32                             UintInstructionCount;
		Math::U32                             StaticFlowControlCount;
		Math::U32                             DynamicFlowControlCount;
		std::string                           Creator;
		std::string                           Name;
	};

	// Class containing the vertex and pixel shader. Supports compiling and hot reloading
	class GPUShader
	{
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

		std::string                GetName()       const noexcept { return m_shaderName; }
		const GPUShaderReflection& GetReflection() const noexcept{ return m_reflection; }

		GPUShaderCompileInfo HotReload();
		void Bind();

		GPUShaderCompileInfo CompileShader(std::string_view path, std::string_view shaderEntry, GPUShaderType type);
		void Release();

	private:
		GPUShaderCompileInfo CompileVertexShader(ID3D11Device* device, std::string_view path, std::string_view shaderEntry);
		GPUShaderCompileInfo CompilePixelShader(ID3D11Device* device, std::string_view path, std::string_view shaderEntry);
		GPUShaderCompileInfo CreateInputLayoutFromVS(ID3D11Device* device);
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
	};
}