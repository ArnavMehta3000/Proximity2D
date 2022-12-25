#pragma once
#include "Graphics/GPUShader.h"

namespace Proximity::Modules
{
	class ShaderLibrary : public Utils::IService
	{
	public:
		ShaderLibrary() = default;

		Math::U32 Count() { return (Math::U32)m_shaders.size(); }

		void AddShader(const std::shared_ptr<Graphics::GPUShader>& shader);
		void AddShader(std::string shaderName, std::string_view path, std::string_view shaderEntry, Graphics::GPUShaderType type);
		void RemoveShader(const std::string& shaderName);

		bool Exists(const std::string& shaderName);

		std::shared_ptr<Graphics::GPUShader> Get(const std::string& shaderName);

		void Release();

	private:
		std::unordered_map<std::string, std::shared_ptr<Graphics::GPUShader>> m_shaders;
	};
}