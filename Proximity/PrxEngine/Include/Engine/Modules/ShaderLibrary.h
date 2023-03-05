#pragma once
#include "Graphics/GPUShader.h"

namespace Proximity::Modules
{
	class ShaderLibrary : public Utils::IService
	{
		using ShaderMap = std::unordered_map<std::string, std::shared_ptr<Graphics::GPUShader>>;

	public:
		ShaderLibrary() = default;

		void BindDefaults();

		inline const Math::U64 Count()   const noexcept            { return (Math::U32)m_shaders.size(); }
		inline const ShaderMap& GetMap() const noexcept            { return m_shaders; }
		inline void UpdateSelected(std::string_view shader)        { m_selectedShader = shader; OnShaderSelected(shader); }
		inline const std::string& GetSelectedName() const noexcept { return m_selectedShader; }

		// Adds a shader by reference
		void AddShader(const std::shared_ptr<Graphics::GPUShader>& shader);
		// Compiles and adds a shader to the library
		void AddShader(std::string shaderName, std::string_view path, std::string_view shaderEntry, Graphics::GPUShaderType type);
		// Removes the shader from the library
		void RemoveShader(const std::string& shaderName);

		// Applies a shader
		void SetShader(const std::string& name);

		// Sets active shaders again
		void SetActive();

		// Checks if a shader exists in the library
		bool Exists(const std::string& shaderName);

		// Hot reloads all shaders in library
		std::string HotReloadAll();
		std::string HotReload(const std::string& name);

		// Retrieves a shader from the library
		std::shared_ptr<Graphics::GPUShader> Get(const std::string& shaderName);
		
		void Release();

	public:
		Utils::Action<std::string_view> OnShaderSelected;

	private:
		Graphics::GPUShader* m_activeVS;
		Graphics::GPUShader* m_activePS;

		std::string m_selectedShader;
		ShaderMap m_shaders;
	};
}