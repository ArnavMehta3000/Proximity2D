#include "enginepch.h"
#include "Engine/Modules/ShaderLibrary.h"

namespace Proximity::Modules
{
	void ShaderLibrary::BindDefaults()
	{
		m_shaders["Internal VS"]->Bind();
		m_shaders["Internal PS"]->Bind();
	}
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
			auto s = m_shaders[name].get();
			s->Bind();
			auto type = s->GetReflection().Type;

			switch (type)
			{
			case Proximity::Graphics::GPUShaderType::None:
				break;
			case Proximity::Graphics::GPUShaderType::Vertex:
				m_activeVS = s;
				break;
			case Proximity::Graphics::GPUShaderType::Pixel:
				m_activePS = s;
				break;
			default:
				break;
			}
		}
		else
			return;
	}

	void ShaderLibrary::SetActive()
	{
		if (m_activePS)
			m_activePS->Bind();
		if (m_activeVS)
			m_activeVS->Bind();
	}

	bool ShaderLibrary::Exists(const std::string& shaderName)
	{
		return (m_shaders.find(shaderName) != m_shaders.end());
	}

	std::string ShaderLibrary::HotReloadAll()
	{
		bool flag = true; 

		for (auto& shader : m_shaders)
		{
			Graphics::GPUShaderCompileInfo info = shader.second->HotReload();
			// Check if hot reload failed
			if (FAILED(info.HResult))
			{
				flag = false;
			}
		}

		if (flag)  // No error while hot reloading
		{
			SetActive();
			return std::string("Hot reloaded all shaders!");
		}
		else
			return std::string("SHADER HOT RELOAD ERROR");
	}

	std::string ShaderLibrary::HotReload(const std::string& name)
	{
		if (!Exists(name))
			return std::string("Shader name not found or does not exist!");

		auto info = m_shaders[name]->HotReload();
		if (FAILED(info.HResult))
			return std::string("SHADER HOT RELOAD ERROR");
		else
			return std::string("Hot reloaded ").append(name);
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