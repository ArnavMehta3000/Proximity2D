#include "enginepch.h"
#include "Engine/Modules/ShaderLibrary.h"

namespace Proximity::Modules
{
	void ShaderLibrary::AddShader(const std::shared_ptr<Graphics::GPUShader>& shader)
	{
		auto& name = shader->GetName();
		if (Exists(name))
		{
			PRX_LOG_WARN("Shader with the same name [%s] already exists in shader library", name.c_str());
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
		if (m_shaders.erase(shaderName) == 1)
			PRX_LOG_INFO("Successfully removed shader [%s] from library", shaderName.c_str());
		else
			PRX_LOG_INFO("Failed to remove shader [%s] from library", shaderName.c_str());
	}

	void ShaderLibrary::SetShader(const std::string& name)
	{
		if (Exists(name))
		{
			m_shaders[name]->Bind();
		}
		else
			return;
	}

	bool ShaderLibrary::Exists(const std::string& shaderName)
	{
		return (m_shaders.find(shaderName) != m_shaders.end());
	}

	std::string ShaderLibrary::HotReloadAll()
	{
		std::stringstream ss;
		bool flag = true; 
		for (auto& shader : m_shaders)
		{
			auto info = shader.second->HotReload();
			if (!info.Succeeded)
			{
				flag = false;
				ss << "Failed to hot reload shader [" << shader.first << "]\nMessage: " << info.Message << "\n";
			}
		}

		if (flag)  // No error while hot reloading
			return std::string("Hot reloaded all shaders!");
		else
			return ss.str();
	}

	std::string ShaderLibrary::HotReload(const std::string& name)
	{
		std::stringstream ss;
		if (!Exists(name))
			return std::string("Shader name not found or does not exist!");

		auto info = m_shaders[name]->HotReload();
		if (!info.Succeeded)
		{
			ss << "Failed to hot reload shader [" << name << "]\nMessage: " << info.Message << "\n";
			return ss.str();
		}
		else
			return std::string("Hot reloaded shader!");
	}

	std::shared_ptr<Graphics::GPUShader> ShaderLibrary::Get(const std::string& shaderName)
	{
		if (!Exists(shaderName))
		{
			PRX_LOG_ERROR("Shader [%s] not found", shaderName.c_str());
			return nullptr;
		}
		else
			return m_shaders[shaderName];
	}

	void ShaderLibrary::Release()
	{
		// Release all shaders
		std::for_each(m_shaders.begin(), m_shaders.end(),
			[](std::pair<std::string, std::shared_ptr<Graphics::GPUShader>> pair)
			{
				pair.second->Release();
			});
	}
}