#pragma once
#include "Graphics/GPUShader.h"

namespace Proximity::Modules
{
	class ShaderLibrary : public Utils::IService
	{
		using ShaderMap = std::unordered_map<std::string, std::shared_ptr<Graphics::GPUShader>>;

	public:
		ShaderLibrary() = default;

		inline const Math::U32 Count()   const noexcept { return (Math::U32)m_shaders.size(); }
		inline const ShaderMap& GetMap() const noexcept { return m_shaders; }

		// Adds a shader by reference
		void AddShader(const std::shared_ptr<Graphics::GPUShader>& shader);
		// Compiles and adds a shader to the library
		void AddShader(std::string shaderName, std::string_view path, std::string_view shaderEntry, Graphics::GPUShaderType type);
		// Removes the shader from the library
		void RemoveShader(const std::string& shaderName);

		// Applies a shader
		void SetShader(const std::string& name);

		// Checks if a shader exists in the library
		bool Exists(const std::string& shaderName);

		// Hot reloads all shaders in library
		std::string HotReloadAll();
		std::string HotReload(const std::string& name);

		// Retrieves a shader from the library
		std::shared_ptr<Graphics::GPUShader> Get(const std::string& shaderName);

		void Release();

	private:
		ShaderMap m_shaders;
	};
}