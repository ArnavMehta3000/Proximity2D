#pragma once
#include "Engine/Modules/TextureLoader.h"

namespace Proximity::Modules
{
	class TextureLibrary : public Utils::IService
	{
		using TextureMap = std::unordered_map<std::string, std::shared_ptr<Graphics::Texture2D>>;

	public:
		TextureLibrary() = default;

		// Create default white texture
		void Init();
		// Load all project textures
		void InitProjectLib();

		void Refresh();

		void AddTexture(std::shared_ptr<Graphics::Texture2D>& texture);
		void RemoveTexture(const std::string& textureName);
		bool Exists(const std::string& textureName);
		std::shared_ptr<Graphics::Texture2D> Get(const std::string& textureName);
		std::shared_ptr<Graphics::Texture2D> GetDefaultTexture();

		inline const Math::U64 Count()    const noexcept           { return m_textures.size(); }
		inline const TextureMap& GetMap() const noexcept           { return m_textures; }
		inline void UpdateSelected(std::string_view shader)        { m_selectedTexture = shader; OnTextureSelected(shader); }
		inline const std::string& GetSelectedName() const noexcept { return m_selectedTexture; }

		void Release();

	public:
		Utils::Action<std::string_view> OnTextureSelected;

	private:
		std::string m_selectedTexture;
		TextureMap m_textures;
	};
}