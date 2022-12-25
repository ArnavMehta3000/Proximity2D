#include "enginepch.h"
#include "Engine/Modules/ShaderLibrary.h"

namespace Proximity::Modules
{
	void ShaderLibrary::AddShader(const std::shared_ptr<Graphics::GPUShader>& shader)
	{
		auto name = shader->GetName();
		if (Exists(name))
		{
			PRX_LOG_WARN("Shader with the same name (%s) already exists in shader library", name.c_str());
			return;
		}
		m_shaders[name] = shader;
	}

	void ShaderLibrary::AddShader(std::string shaderName, std::string_view path, std::string_view shaderEntry, Graphics::GPUShaderType type)
	{
		if (Exists(shaderName))
		{
			PRX_LOG_WARN("Shader with the same name (%s) already exists in shader library", shaderName.c_str());
			return;
		}

		std::shared_ptr<Graphics::GPUShader> shader;
		shader = std::make_shared< Graphics::GPUShader>(shaderName);
		shader->CompileShader(path, shaderEntry, type);

		m_shaders[shaderName.data()] = shader;
	}

	void ShaderLibrary::RemoveShader(const std::string& shaderName)
	{
		// TODO: Finish remove shader function
	}

	bool ShaderLibrary::Exists(const std::string& shaderName)
	{
		return (m_shaders.find(shaderName) != m_shaders.end());
	}

	std::shared_ptr<Graphics::GPUShader> ShaderLibrary::Get(const std::string& shaderName)
	{
		if (!Exists(shaderName))
		{
			PRX_LOG_ERROR("Shader (%s) not found", shaderName.c_str());
			return nullptr;
		}
		else
			return m_shaders[shaderName];
	}

	void ShaderLibrary::Release()
	{
		for (auto& shader : m_shaders)
		{
			shader.second->Release();
		}
	}
}