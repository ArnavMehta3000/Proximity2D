#include "enginepch.h"
#include "Engine/Modules/TextureLibrary.h"
#include "Graphics/D3DManager.h"
#include <execution>

namespace Proximity::Modules
{
	void TextureLibrary::Init()
	{
		auto whiteTex = std::make_shared<Graphics::Texture2D>();
		whiteTex->Width     = 1;
		whiteTex->Height    = 1;
		whiteTex->TexFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		whiteTex->SrvFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		whiteTex->BindFlags = D3D11_BIND_SHADER_RESOURCE;
		whiteTex->Name      = "_InteralTex";

		static const uint32_t s_pixel = 0xffffffff;
		D3D11_SUBRESOURCE_DATA initData = { &s_pixel, sizeof(uint32_t), 0 };

		CREATE_ZERO(D3D11_TEXTURE2D_DESC, desc);
		desc.Width            = whiteTex->Width;
		desc.Height           = whiteTex->Height;
		desc.MipLevels        = 1;
		desc.ArraySize        = 1;
		desc.Format           = whiteTex->TexFormat;
		desc.SampleDesc.Count = 1;
		desc.Usage            = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags        = whiteTex->BindFlags;

		auto dev = PRX_RESOLVE(Graphics::D3DManager)->GetDevice();
		PRX_ASSERT_HR(dev->CreateTexture2D(&desc, &initData, whiteTex->D3DTexture2D.ReleaseAndGetAddressOf()),
			"Failed to create white texture");

		PRX_ASSERT_MSG(!whiteTex->CreateSRV(),
			"Failed to create shader resource view of white texture");

		PRX_LOG_DEBUG("Created interal white texture");

		AddTexture(whiteTex);
	}

	void TextureLibrary::InitProjectLib()
	{
		PRX_LOG_DEBUG("Initializing project library for textures");
		auto& rootPath = DirectoryManager::s_appDirectories.TexturesPath;
		using recursiveDirIter = std::filesystem::recursive_directory_iterator;

		for (const auto& dir : recursiveDirIter(rootPath))
		{
			auto filename = DirectoryManager::GetFileNameFromDir(dir.path(), false);
			std::shared_ptr<Graphics::Texture2D> ptr(TextureLoader::Load(dir.path().string().c_str(), filename));
			AddTexture(ptr);
		}

		PRX_LOG_DEBUG("Finished initializing project library for textures");
	}

	void TextureLibrary::Refresh()
	{
		bool found = false;
		PRX_LOG_DEBUG("Refreshing project library for textures");

		auto& rootPath = DirectoryManager::s_appDirectories.TexturesPath;
		using recursiveDirIter = std::filesystem::recursive_directory_iterator;

		for (const auto& dir : recursiveDirIter(rootPath))
		{
			auto filename = DirectoryManager::GetFileNameFromDir(dir.path(), false);
			if (!Exists(filename))
			{
				std::shared_ptr<Graphics::Texture2D> ptr(TextureLoader::Load(dir.path().string().c_str(), filename));
				AddTexture(ptr);
				found = true;
			}
			else
			{
				// No new image files
			}
		}

		if (found)
			PRX_LOG_DEBUG("Refreshed texture library - added/removed textures");
		else
			PRX_LOG_DEBUG("Refreshed texture library - no changes");
	}

	void TextureLibrary::AddTexture(std::shared_ptr<Graphics::Texture2D>& texture)
	{
		auto& name = texture->Name;
		if (Exists(name))
		{
			PRX_LOG_WARN("Texture with the same name [%s] already exists in texture library", name.c_str());
			return;
		}
		m_textures[name] = texture;
	}

	void TextureLibrary::RemoveTexture(const std::string& textureName)
	{
		if (m_textures.erase(textureName) == 1)
			PRX_LOG_INFO("Successfully removed texture [%s] from library", textureName.c_str());
		else
			PRX_LOG_INFO("Failed to remove texture [%s] from library", textureName.c_str());
	}

	bool TextureLibrary::Exists(const std::string& textureName)
	{
		return (m_textures.find(textureName) != m_textures.end());
	}

	std::shared_ptr<Graphics::Texture2D> TextureLibrary::Get(const std::string& textureName)
	{
		if (!Exists(textureName))
		{
			PRX_LOG_ERROR("Texture [%s] not found", textureName.c_str());
			return nullptr;
		}
		else
			return m_textures[textureName];
	}

	std::shared_ptr<Graphics::Texture2D> TextureLibrary::GetDefaultTexture()
	{
		return Get("_InteralTex");
	}

	void TextureLibrary::Release()
	{
		std::for_each(std::execution::par, m_textures.begin(), m_textures.end(),
			[](std::pair<std::string, std::shared_ptr<Graphics::Texture2D>> pair)
			{
				pair.second->Release();
			});
	}
}