#include "enginepch.h"
#include "Graphics/GPUShader.h"
#include "Graphics/D3DManager.h"
#include "Engine/EngineGlobals.h"

namespace Proximity::Graphics
{
	static const char* VS_MODEL = "vs_5_0";
	static const char* PS_MODEL = "ps_5_0";

	GPUShader::GPUShader(std::string_view shaderName)
		:
		m_shaderName(shaderName.data()),
		m_shaderType(GPUShaderType::None),
		m_vertexShader(),
		m_pixelShader(),
		m_reflection(),
		m_reflector(nullptr)
	{
	}

	void GPUShader::Release()
	{
		COM_RELEASE(m_vertexShader.Shader);
		COM_RELEASE(m_vertexShader.InputLayout);
		COM_RELEASE(m_pixelShader.Shader);
	}

	GPUShaderCompileInfo GPUShader::HotReload()
	{
		return this->CompileShader(m_filePath, m_entrypoint, m_shaderType);
	}

	void GPUShader::Bind()
	{
		auto d3d = Core::Globals::g_engineServices.ResolveService<Graphics::D3DManager>();
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
		m_filePath = path;
		m_shaderType = type;
		m_entrypoint = shaderEntry;

		GPUShaderCompileInfo info{};

		auto d3d = Core::Globals::g_engineServices.ResolveService<Graphics::D3DManager>();
		if (d3d == nullptr)
		{
			info.HResult = E_FAIL;
			info.Succeeded = false;
			info.Message = "Failed to resolve Graphics::D3DManager";
			return info;
		}

		switch (m_shaderType)
		{
		case Proximity::Graphics::GPUShaderType::None:
			info.HResult   = E_INVALIDARG;
			info.Succeeded = false;
			info.Message   = "Failed to create GPU Shader. Error: Shader type set GPUShader::None";
			return info;
		
		case Proximity::Graphics::GPUShaderType::Vertex:
			info = CompileVertexShader(d3d->GetDevice(), path, shaderEntry);

		case Proximity::Graphics::GPUShaderType::Pixel:
			info = CompilePixelShader(d3d->GetDevice(), path, shaderEntry);
		}

		CreateReflection();

		return info;
	}

	GPUShaderCompileInfo GPUShader::CompileVertexShader(ID3D11Device* device, std::string_view path, std::string_view shaderEntry)
	{
		GPUShaderCompileInfo info{};

		std::string compileError = "Failed to compile vertex shader: " + std::string(path.data()) + "\nERROR: ";
		std::string compileWarn = "Compiled with warnings vertex shader: " + std::string(path.data()) + "\nWARNING: ";
		std::string createError = "Failed to create vertex shader: " + std::string(path.data());


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
			VS_MODEL,
			shaderFlags,
			0,
			shaderBlob.ReleaseAndGetAddressOf(),
			errorBlob.ReleaseAndGetAddressOf());


		if (FAILED(hr))  // Failed to compile
		{
			if (errorBlob)  // Contains error message
			{
				const char* errorMsg = reinterpret_cast<const char*> (errorBlob->GetBufferPointer());

				info.HResult = hr;
				info.Succeeded = false;
				info.Message = compileError + std::string(errorMsg);

				COM_RELEASE(errorBlob);

				return info;
			}
		}

		// Check for compile warnings
		if (errorBlob)
		{
			const char* errorMsg = reinterpret_cast<const char*> (errorBlob->GetBufferPointer());

			info.HResult = hr;
			info.Succeeded = true;
			info.Message = compileWarn + std::string(errorMsg);

			COM_RELEASE(errorBlob);
		}


		// Create vertex shader
		m_vertexShader.Blob = shaderBlob;
		hr = device->CreateVertexShader(m_vertexShader.Blob->GetBufferPointer(), m_vertexShader.Blob->GetBufferSize(), nullptr, m_vertexShader.Shader.ReleaseAndGetAddressOf());
		if (FAILED(hr))
		{
			info.HResult   = hr;
			info.Succeeded = false;
			info.Message   = createError;

			return info;
		}

		// Create input layout from vertex shader
		info = CreateInputLayoutFromVS(device);

		return info;
	}

	GPUShaderCompileInfo GPUShader::CreateInputLayoutFromVS(ID3D11Device* device)
	{
		// Ref: https://learn.microsoft.com/en-us/windows/win32/api/d3d11shader/nn-d3d11shader-id3d11shaderreflection
		// Macro usage from https://learn.microsoft.com/en-us/windows/win32/api/combaseapi/nf-combaseapi-iid_ppv_args

		GPUShaderCompileInfo info{};
		HRESULT hr = E_FAIL;

		ComPtr<ID3D11ShaderReflection> vsReflection = nullptr;
		hr = D3DReflect(m_vertexShader.Blob->GetBufferPointer(), m_vertexShader.Blob->GetBufferSize(), IID_PPV_ARGS(vsReflection.ReleaseAndGetAddressOf()));
		if (FAILED(hr))
		{
			info.HResult = hr;
			info.Succeeded = false;
			info.Message = "Failed to create shader reflection of vertex shader: " + m_filePath;
			return info;
		}

		CREATE_ZERO(D3D11_SHADER_DESC, desc);
		hr = vsReflection->GetDesc(&desc);
		if (FAILED(hr))
		{
			info.HResult = hr;
			info.Succeeded = false;
			info.Message = "Failed to get reflection description of vertex shader: " + m_filePath;
			return info;
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

		if (FAILED(hr))
		{
			info.HResult = hr;
			info.Succeeded = false;
			info.Message = "Failed to create input layout from vertex shader: " + m_filePath;
		}
		else
		{
			info.HResult = hr;
			info.Succeeded = true;
			info.Message = "Successfully created input layout from vertex shader: " + m_filePath;
		}

		return info;
	}

	GPUShaderCompileInfo GPUShader::CompilePixelShader(ID3D11Device* device, std::string_view path, std::string_view shaderEntry)
	{
		GPUShaderCompileInfo info{};

		std::string compileError = "Failed to compile vertex shader: " + std::string(path.data()) + "\nERROR: ";
		std::string compileWarn = "Compiled with warnings vertex shader: " + std::string(path.data()) + "\nWARNING: ";
		std::string createError = "Failed to create vertex shader: " + std::string(path.data());


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


		if (FAILED(hr))  // Failed to compile
		{
			if (errorBlob)  // Contains error message
			{
				const char* errorMsg = reinterpret_cast<const char*> (errorBlob->GetBufferPointer());

				info.HResult = hr;
				info.Succeeded = false;
				info.Message = compileError + std::string(errorMsg);

				COM_RELEASE(errorBlob);

				return info;
			}
		}

		// Check for compile warnings
		if (errorBlob)
		{
			const char* errorMsg = reinterpret_cast<const char*> (errorBlob->GetBufferPointer());

			info.HResult   = hr;
			info.Succeeded = true;
			info.Message   = compileWarn + std::string(errorMsg);

			COM_RELEASE(errorBlob);
		}

		// Create pixel shader
		m_pixelShader.Blob = shaderBlob;
		hr = device->CreatePixelShader(m_pixelShader.Blob->GetBufferPointer(), m_pixelShader.Blob->GetBufferSize(), nullptr, m_pixelShader.Shader.ReleaseAndGetAddressOf());

		if (FAILED(hr))
		{
			info.HResult   = hr;
			info.Succeeded = false;
			info.Message   = createError;
		}
		else
		{
			info.HResult   = hr;
			info.Succeeded = true;
			info.Message = "Successfully created pixel shader from file: " + m_filePath;
		}

		return info;
	}

	void GPUShader::CreateReflection()
	{
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

		// To generate constant buffer

		//if (shaderDesc.ConstantBuffers > 0)
		//{
		//	// Loop over constant buffers
		//	for (Math::U32 i = 0; i < shaderDesc.ConstantBuffers; i++)
		//	{
		//		CREATE_ZERO(ConstantBufferReflection, buf);
		//		buf.CBReflection = reflector->GetConstantBufferByIndex(i);

		//		CREATE_ZERO(D3D11_SHADER_BUFFER_DESC, cbDesc);
		//		buf.CBReflection->GetDesc(&cbDesc);
		//		
		//		// Loop over variables in constant buffer
		//		for (UINT j = 0; j < cbDesc.Variables; j++)
		//		{
		//			buf.CBVariables.push_back(reflector->GetConstantBufferByIndex(i)->GetVariableByIndex(j));
		//		}

		//		m_reflection.ConstantBufferReflections.push_back(buf);
		//	}
		//}
	}
}