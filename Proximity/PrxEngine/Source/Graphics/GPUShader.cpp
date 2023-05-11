#include "enginepch.h"
#include "Graphics/GPUShader.h"
#include "Graphics/D3DManager.h"
#include "Engine/EngineGlobals.h"
#include "Engine/Modules/ShaderLibrary.h"
#include "optick/include/optick.h"

namespace Proximity::Graphics
{
	static const char* VS_MODEL = "vs_5_0";
	static const char* PS_MODEL = "ps_5_0";

#pragma region Internal Shaders
	static const std::string INTERNAL_VS =
		R"(
	struct VSInput
	{
		float3 Position : POSITION;
		float2 TexCoord : COLOR;
	};

	struct VSOutput
	{
		float4 Position : SV_POSITION;
		float2 TexCoord : COLOR;
	};

	cbuffer CameraMatrices : register(b0)
	{
	    matrix WorldMatrix;
	    matrix ViewMatrix;
	    matrix ProjectionMatrix;
	};

	VSOutput VSmain(VSInput input)
	{
	    VSOutput output;
	    output.Position = float4(input.Position, 1.0f);

	    output.Position = mul(output.Position, WorldMatrix);
	    output.Position = mul(output.Position, ViewMatrix);
	    output.Position = mul(output.Position, ProjectionMatrix);

	    output.TexCoord = input.TexCoord;

	    return output;
	}

		)";

	// TODO: WVP multiplication in the internal VS
	static const std::string INTERNAL_PS =
		R"(
	struct VSInput
	{
		float3 Position : POSITION;
		float2 TexCoord : COLOR;
	};

	Texture2D tex               : register(t0);
	SamplerState textureSampler : register(s0);

	cbuffer Properties : register (b0)
	{
		float4 Tint = float4(1.0f, 1.0f, 1.0f, 1.0f);
		bool UseTextures = true;
	}

	struct VSOutput
	{
		float4 Position : SV_POSITION;
		float2 TexCoord : COLOR;
	};

	float4 PSmain(VSOutput input) : SV_TARGET
	{
		if (UseTextures)
			return Tint * tex.Sample(textureSampler, input.TexCoord);
		else
			return Tint;
	})";
#pragma endregion

	const std::string GPUShader::TypeToString(GPUShaderType type) noexcept
	{
		switch (type)
		{
		case Proximity::Graphics::GPUShaderType::Vertex:
			return "Vertex";
		case Proximity::Graphics::GPUShaderType::Pixel:
			return "Pixel";
		
		case Proximity::Graphics::GPUShaderType::None:
			return "None";
		default:
			return "None";
		}
	}

	GPUShaderType GPUShader::StringToType(const std::string& type) noexcept
	{
		if (type == "Vertex")
			return GPUShaderType::Vertex;
		if (type == "Pixel")
			return GPUShaderType::Pixel;
		else
			return GPUShaderType::None;
	}


	GPUShader::GPUShader(std::string_view shaderName)
		:
		m_shaderName(shaderName.data()),
		m_shaderType(GPUShaderType::None),
		m_vertexShader(),
		m_pixelShader(),
		m_reflection(),
		m_reflector(nullptr),
		m_isInternal(false)
	{
	}

	void GPUShader::CreateDefaults()
	{
		OPTICK_EVENT("GPUShader::CreateDefaults")
		PRX_LOG_DEBUG("Creating default internal shaders");

		auto lib = PRX_RESOLVE(Modules::ShaderLibrary);
		auto d3d = PRX_RESOLVE(Graphics::D3DManager);

		DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
		shaderFlags |= D3DCOMPILE_DEBUG;
		shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif // _DEBUG
		

		// Create vertex shader
		{
			GPUShader vs("Internal VS");

			vs.m_isInternal      = true;
			vs.m_filePath        = "INTERNAL";
			vs.m_shaderType      = GPUShaderType::Vertex;
			vs.m_entrypoint      = "VSmain";

			ComPtr<ID3DBlob> errorBlob = nullptr;
			ComPtr<ID3DBlob> shaderBlob = nullptr;

			// Compile vertex shader
			HRESULT hr = D3DCompile(
				INTERNAL_VS.c_str(),
				INTERNAL_VS.length(),
				NULL,
				NULL,
				D3D_COMPILE_STANDARD_FILE_INCLUDE,
				vs.m_entrypoint.c_str(),
				VS_MODEL,
				shaderFlags,
				0,
				shaderBlob.ReleaseAndGetAddressOf(),
				errorBlob.ReleaseAndGetAddressOf());

			PRX_ASSERT_HR(hr, "Failed internal D3DCompileVS");

			if (FAILED(hr))  // Failed to compile
			{
				if (errorBlob)  // Contains error message
				{
					const char* errorMsg = reinterpret_cast<const char*> (errorBlob->GetBufferPointer());
					__debugbreak();
				}
			}
			GPUShaderCompileInfo info{};

			vs.m_vertexShader.Blob = shaderBlob;
			hr = d3d->GetDevice()->CreateVertexShader(vs.m_vertexShader.Blob->GetBufferPointer(), vs.m_vertexShader.Blob->GetBufferSize(), nullptr, vs.m_vertexShader.Shader.ReleaseAndGetAddressOf());
			vs.CreateInputLayoutFromVS(d3d->GetDevice(), info);
			vs.CreateReflection();

			lib->AddShader(std::make_shared<Graphics::GPUShader>(vs));
		}

		// Create pixel shader
		{
			GPUShader ps("Internal PS");

			ps.m_isInternal      = true;
			ps.m_filePath        = "INTERNAL";
			ps.m_shaderType      = GPUShaderType::Pixel;
			ps.m_entrypoint      = "PSmain";

			ComPtr<ID3DBlob> errorBlob = nullptr;
			ComPtr<ID3DBlob> shaderBlob = nullptr;

			// Compile pixel shader
			HRESULT hr = D3DCompile(
				INTERNAL_PS.c_str(),
				INTERNAL_PS.length(),
				NULL,
				NULL,
				D3D_COMPILE_STANDARD_FILE_INCLUDE,
				ps.m_entrypoint.c_str(),
				PS_MODEL,
				shaderFlags,
				0,
				shaderBlob.ReleaseAndGetAddressOf(),
				errorBlob.ReleaseAndGetAddressOf());

			PRX_ASSERT_HR(hr, "Failed internal D3DCompilePS");

			if (FAILED(hr))  // Failed to compile
			{
				if (errorBlob)  // Contains error message
				{
					const char* errorMsg = reinterpret_cast<const char*> (errorBlob->GetBufferPointer());
					__debugbreak();
				}
			}

			ps.m_pixelShader.Blob = shaderBlob;
			hr = d3d->GetDevice()->CreatePixelShader(ps.m_pixelShader.Blob->GetBufferPointer(), ps.m_pixelShader.Blob->GetBufferSize(), nullptr, ps.m_pixelShader.Shader.ReleaseAndGetAddressOf());
			ps.CreateReflection();

			lib->AddShader(std::make_shared<Graphics::GPUShader>(ps));
		}

		lib->BindDefaults();
	}

	void GPUShader::Release()
	{
		COM_RELEASE(m_vertexShader.Shader);
		COM_RELEASE(m_vertexShader.InputLayout);
		COM_RELEASE(m_pixelShader.Shader);
	}

	GPUShaderCompileInfo GPUShader::HotReload()
	{
		OPTICK_EVENT("GPUShader::HotReload")
		if (m_isInternal)
		{
			GPUShaderCompileInfo info{};
			info.HResult = S_OK;
			info.Message << "Ignoring hot reload...internal Shader";
			return info;
		}
		return this->CompileShader(m_filePath, m_entrypoint, m_shaderType);
	}

	void GPUShader::Bind()
	{
		auto d3d = PRX_RESOLVE(Graphics::D3DManager);
		switch (m_shaderType)
		{
		case Proximity::Graphics::GPUShaderType::None:
			d3d->GetContext()->IASetInputLayout(nullptr);
			d3d->GetContext()->VSSetShader(nullptr, nullptr, 0);
			d3d->GetContext()->PSSetShader(nullptr, nullptr, 0);
			break;
		case Proximity::Graphics::GPUShaderType::Vertex:
			d3d->GetContext()->IASetInputLayout(m_vertexShader.InputLayout.Get());
			d3d->GetContext()->VSSetShader(m_vertexShader.Shader.Get(), nullptr, 0);
			break;
		case Proximity::Graphics::GPUShaderType::Pixel:
			d3d->GetContext()->PSSetShader(m_pixelShader.Shader.Get(), nullptr, 0);
			break;
		default:
			break;
		}
	}

	GPUShaderCompileInfo GPUShader::CompileShader(std::string_view path, std::string_view shaderEntry, GPUShaderType type)
	{
		OPTICK_EVENT("GPUShader::CompileShader")
		m_filePath   = path;
		m_shaderType = type;
		m_entrypoint = shaderEntry;

		GPUShaderCompileInfo info{};

		auto d3d = PRX_RESOLVE(Graphics::D3DManager);
		if (d3d == nullptr)
		{
			info.HResult = E_FAIL;
			info.Message << "Failed to resolve Graphics::D3DManager";
			return info;
		}

		switch (m_shaderType)
		{
		case Proximity::Graphics::GPUShaderType::None:
			info.HResult   = E_INVALIDARG;
			info.Message << "Failed to create GPU Shader. Error: Shader type set GPUShader::None";
			return info;
		
		case Proximity::Graphics::GPUShaderType::Vertex:
			info = CompileVertexShader(d3d->GetDevice(), path, shaderEntry);
			PRX_LOG_INFO("Vertex Shader Compile Info:\n%s", info.Message.str().c_str());

		case Proximity::Graphics::GPUShaderType::Pixel:
			info = CompilePixelShader(d3d->GetDevice(), path, shaderEntry);
			PRX_LOG_INFO("Pixel Shader Compile Info:\n%s", info.Message.str().c_str());
		}

		CreateReflection();

		return info;
	}

	GPUShaderCompileInfo GPUShader::CompileVertexShader(ID3D11Device* device, std::string_view path, std::string_view shaderEntry)
	{
		GPUShaderCompileInfo info{};

		std::string compileError = "Failed to compile vertex shader: " + std::string(path.data()) + "\nERROR: ";
		std::string compileWarn  = "Compiled with warnings vertex shader: " + std::string(path.data()) + "\nWARNING: ";
		std::string createError  = "Failed to create vertex shader: " + std::string(path.data());


		DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
		shaderFlags |= D3DCOMPILE_DEBUG;
		shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif // _DEBUG

		ComPtr<ID3DBlob> errorBlob = nullptr;
		ComPtr<ID3DBlob> shaderBlob = nullptr;

		// Compile vertex shader
		HRESULT hr = D3DCompileFromFile(
			Utils::ToWideString(path.data()).c_str(),
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			shaderEntry.data(),
			VS_MODEL,
			shaderFlags,
			0,
			shaderBlob.ReleaseAndGetAddressOf(),
			errorBlob.ReleaseAndGetAddressOf());


		if (FAILED(hr) || shaderBlob == nullptr)  // Failed to compile
		{
			info.HResult = hr;
			info.Message << compileError;
			if (errorBlob)  // Contains error message
			{
				const char* errorMsg = reinterpret_cast<const char*> (errorBlob->GetBufferPointer());
				info.Message << errorMsg;
				COM_RELEASE(errorBlob);

			}
			// Failed to compile shader return
			return info;
		}

		// Compilation did not fail - Check for compile warnings
		if (errorBlob)
		{
			const char* errorMsg = reinterpret_cast<const char*> (errorBlob->GetBufferPointer());

			info.HResult = hr;
			info.Message << compileWarn << errorMsg;

			COM_RELEASE(errorBlob);
		}


		// Create vertex shader
		m_vertexShader.Blob = shaderBlob;
		hr = device->CreateVertexShader(m_vertexShader.Blob->GetBufferPointer(), m_vertexShader.Blob->GetBufferSize(), nullptr, m_vertexShader.Shader.ReleaseAndGetAddressOf());
		if (FAILED(hr))
		{
			info.HResult = hr;
			info.Message << createError;

			// D3D error creating vertex shader. Failed [return from here]
			return info;
		}

		// Create input layout from vertex shader
		CreateInputLayoutFromVS(device, info);

		return info;
	}

	void  GPUShader::CreateInputLayoutFromVS(ID3D11Device* device, GPUShaderCompileInfo& info)
	{
		// Ref: https://learn.microsoft.com/en-us/windows/win32/api/d3d11shader/nn-d3d11shader-id3d11shaderreflection
		// Macro usage from https://learn.microsoft.com/en-us/windows/win32/api/combaseapi/nf-combaseapi-iid_ppv_args

		HRESULT hr = E_FAIL;

		ComPtr<ID3D11ShaderReflection> vsReflection = nullptr;
		hr = D3DReflect(m_vertexShader.Blob->GetBufferPointer(), m_vertexShader.Blob->GetBufferSize(), IID_PPV_ARGS(vsReflection.ReleaseAndGetAddressOf()));
		if (FAILED(hr))
		{
			info.HResult = hr;
			info.Message << "Failed to create shader reflection of vertex shader: " << m_filePath;
			return;
		}

		CREATE_ZERO(D3D11_SHADER_DESC, desc);
		hr = vsReflection->GetDesc(&desc);
		if (FAILED(hr))
		{
			info.HResult = hr;
			info.Message << "Failed to get reflection description of vertex shader: " << m_filePath;
			return;
		}

		std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayout;
		for (UINT i = 0; i < desc.InputParameters; i++)
		{
			// Get input parameter at index
			CREATE_ZERO(D3D11_SIGNATURE_PARAMETER_DESC, paramDesc);
			vsReflection->GetInputParameterDesc(i, &paramDesc);

			// Create input element descripton
			CREATE_ZERO(D3D11_INPUT_ELEMENT_DESC, elementDesc);
			elementDesc.SemanticName         = paramDesc.SemanticName;
			elementDesc.SemanticIndex        = paramDesc.SemanticIndex;
			elementDesc.InputSlot            = 0;
			elementDesc.AlignedByteOffset    = D3D11_APPEND_ALIGNED_ELEMENT;
			elementDesc.InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
			elementDesc.InstanceDataStepRate = 0;

			// Determine DXGI format
			if (paramDesc.Mask == 1)
			{
				if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
					elementDesc.Format = DXGI_FORMAT_R32_UINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
					elementDesc.Format = DXGI_FORMAT_R32_SINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
					elementDesc.Format = DXGI_FORMAT_R32_FLOAT;
			}
			else if (paramDesc.Mask <= 3)
			{
				if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
					elementDesc.Format = DXGI_FORMAT_R32G32_UINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
					elementDesc.Format = DXGI_FORMAT_R32G32_SINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
					elementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
			}
			else if (paramDesc.Mask <= 7)
			{
				if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
					elementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
					elementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
					elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
			}
			else if (paramDesc.Mask <= 15)
			{
				if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
					elementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
					elementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
					elementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			}

			inputLayout.push_back(elementDesc);
		}


		hr = device->CreateInputLayout(
			&inputLayout[0],
			(UINT)inputLayout.size(),
			m_vertexShader.Blob->GetBufferPointer(),
			m_vertexShader.Blob->GetBufferSize(),
			m_vertexShader.InputLayout.ReleaseAndGetAddressOf());

		info.HResult = hr;
		if (FAILED(hr))
			info.Message << "Failed to create input layout from vertex shader: " << m_filePath;
		else
			info.Message << "Successfully created input layout from vertex shader: " << m_filePath;
	}

	GPUShaderCompileInfo GPUShader::CompilePixelShader(ID3D11Device* device, std::string_view path, std::string_view shaderEntry)
	{
		GPUShaderCompileInfo info{};

		std::string compileError = "Failed to compile pixel shader: " + std::string(path.data()) + "\nERROR: ";
		std::string compileWarn  = "Compiled with warnings pixel shader: " + std::string(path.data()) + "\nWARNING: ";
		std::string createError  = "Failed to create pixel shader: " + std::string(path.data());


		DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
		shaderFlags |= D3DCOMPILE_DEBUG;
		shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif // _DEBUG

		ComPtr<ID3DBlob> errorBlob = nullptr;
		ComPtr<ID3DBlob> shaderBlob = nullptr;

		// Coimpile vertex shader
		HRESULT hr = D3DCompileFromFile(
			Utils::ToWideString(path.data()).c_str(),
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			shaderEntry.data(),
			PS_MODEL,
			shaderFlags,
			0,
			shaderBlob.ReleaseAndGetAddressOf(),
			errorBlob.ReleaseAndGetAddressOf());


		if (FAILED(hr) || shaderBlob == nullptr)  // Failed to compile
		{
			info.HResult = hr;
			info.Message << compileError;
			if (errorBlob)  // Contains error message
			{
				const char* errorMsg = reinterpret_cast<const char*> (errorBlob->GetBufferPointer());
				info.Message << errorMsg;
				COM_RELEASE(errorBlob);

			}
			// Failed to compile shader return
			return info;
		}

		// Compilation did not fail - Check for compile warnings
		if (errorBlob)
		{
			const char* errorMsg = reinterpret_cast<const char*> (errorBlob->GetBufferPointer());

			info.HResult   = hr;
			info.Message << compileWarn + std::string(errorMsg);

			COM_RELEASE(errorBlob);
		}

		// Create pixel shader
		m_pixelShader.Blob = shaderBlob;
		hr = device->CreatePixelShader(m_pixelShader.Blob->GetBufferPointer(), m_pixelShader.Blob->GetBufferSize(), nullptr, m_pixelShader.Shader.ReleaseAndGetAddressOf());

		if (FAILED(hr))
		{
			info.HResult = hr;
			info.Message << createError;
		}
		else
		{
			info.HResult = hr;
			info.Message << "Successfully compiled pixel shader from file: " << m_filePath;
		}

		return info;
	}

	void GPUShader::CreateReflection()
	{
		OPTICK_EVENT("GPUShader::CreateReflection")
		HRESULT hr = E_FAIL;
		switch (m_shaderType)
		{
		case Proximity::Graphics::GPUShaderType::None:
			return;

		case Proximity::Graphics::GPUShaderType::Vertex:
			hr = D3DReflect(m_vertexShader.Blob->GetBufferPointer(), m_vertexShader.Blob->GetBufferSize(), IID_PPV_ARGS(m_reflector.ReleaseAndGetAddressOf()));
			break;

		case Proximity::Graphics::GPUShaderType::Pixel:
			hr = D3DReflect(m_pixelShader.Blob->GetBufferPointer(), m_pixelShader.Blob->GetBufferSize(), IID_PPV_ARGS(m_reflector.ReleaseAndGetAddressOf()));
			break;
		}
		PRX_ASSERT_HR(hr, "Failed to reflect shader");

		CREATE_ZERO(D3D11_SHADER_DESC, shaderDesc);
		m_reflector->GetDesc(&shaderDesc);

		m_reflection.Name                        = m_shaderName;
		m_reflection.Version                     = shaderDesc.Version;
		m_reflection.Creator                     = shaderDesc.Creator;
		m_reflection.ConstantBuffersCount        = shaderDesc.ConstantBuffers;
		m_reflection.BoundResources              = shaderDesc.BoundResources;
		m_reflection.InputParameters             = shaderDesc.InputParameters;
		m_reflection.TextureLoadInstructions     = shaderDesc.TextureLoadInstructions;
		m_reflection.TextureCompInstructions     = shaderDesc.TextureCompInstructions;
		m_reflection.TextureBiasInstructions     = shaderDesc.TextureBiasInstructions;
		m_reflection.TextureGradientInstructions = shaderDesc.TextureGradientInstructions;
		m_reflection.TextureStoreInstructions    = shaderDesc.cTextureStoreInstructions;
		m_reflection.TextureNormalInstructions   = shaderDesc.TextureNormalInstructions;
		m_reflection.FloatInstructionCount       = shaderDesc.FloatInstructionCount;
		m_reflection.IntInstructionCount         = shaderDesc.IntInstructionCount;
		m_reflection.UintInstructionCount        = shaderDesc.UintInstructionCount;
		m_reflection.StaticFlowControlCount      = shaderDesc.StaticFlowControlCount;
		m_reflection.TextureBiasInstructions     = shaderDesc.DynamicFlowControlCount;
	}
}