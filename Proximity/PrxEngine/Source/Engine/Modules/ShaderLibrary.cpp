#include "enginepch.h"
#include "Engine/Modules/ShaderLibrary.h"
#include "Engine/Modules/MaterialLibrary.h"
#include "Utils/DirectoryManager.h"
#include "yaml-cpp/yaml.h"

namespace Proximity::Modules
{
	void ShaderLibrary::InitProjectLib()
	{
		YAML::Node data = YAML::LoadFile(Utils::DirectoryManager::s_appDirectories.ShaderDataFile.string());
		if (!data["Shaders"])
		{
			return;
		}


		for (auto material : data["Shaders"])
		{
			auto name     = material["Name"].as<std::string>();
			auto typeStr  = material["Type"].as<std::string>();
			auto entry    = material["Entry Point"].as<std::string>();
			auto filepath = material["Filepath"].as<std::string>();

			auto type = Graphics::GPUShader::StringToType(typeStr);
			
			if (type != Graphics::GPUShaderType::None)
				AddShader(name, filepath, entry, type);
		}
	}

	void ShaderLibrary::SerializeShaders()
	{
		auto Serializer = [](YAML::Emitter& out, std::pair<std::string, std::shared_ptr<Graphics::GPUShader>>& pair)
		{
			auto const& shader = pair.second;

			out << YAML::BeginMap;

			out << YAML::Key << "Name" << YAML::Value << shader->GetName();
			out << YAML::Key << "Type" << YAML::Value << shader->GetTypeAsString();
			out << YAML::Key << "Entry Point" << YAML::Value << shader->GetEntry();
			out << YAML::Key << "Filepath" << YAML::Value << shader->GetFilepath();

			out << YAML::EndMap;

		};

		std::filesystem::path outFilePath = Utils::DirectoryManager::s_appDirectories.ShaderDataFile;
		YAML::Emitter out;

		out << YAML::BeginMap;
		out << YAML::Key << "Shaders" << YAML::Value << YAML::BeginSeq;

		std::for_each(m_shaders.begin(), m_shaders.end(),
			[&](std::pair<std::string, std::shared_ptr<Graphics::GPUShader>> pair)
			{
				if (pair.first == "Internal VS" || pair.first == "Internal PS")
					return;

				Serializer(out, pair);
			});
		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(outFilePath);
		fout << out.c_str();
		fout.close();
	}

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
			auto type = s->GetType();

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

		for (auto& [shaderName, shader]: m_shaders)
		{
			Graphics::GPUShaderCompileInfo info = shader->HotReload();
			// Check if hot reload failed
			if (FAILED(info.HResult))
			{
				flag = false;
			}
			else
			{
				// Update the material library shaders to reflect CB's
				auto matLib = PRX_RESOLVE(Modules::MaterialLibrary);
				for (auto& [materialName, material] : matLib->GetMap())
				{
					auto& [vertexShader, pixelShader] = material->GetShaderPair();
					if (vertexShader->GetName() == shaderName || pixelShader->GetName() == shaderName)
					{
						material->HotReload();
					}
				}
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