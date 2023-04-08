#include "enginepch.h"
#include "Engine/Modules/TextureLoader.h"
#include "Graphics/D3DManager.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

namespace Proximity::Modules
{
	Graphics::Texture2D* TextureLoader::Load(const char* filename, const std::string& name)
	{
		Timer loadTimer;
		Graphics::Texture2D* tex = new Graphics::Texture2D();

		int width = 0, height = 0, channels = 0, desiredChannels = 4;
		auto imageData = stbi_load(filename, &width, &height, &channels, desiredChannels);
		if (!imageData)
		{
			PRX_ASSERT_MSG(imageData, "Failed to load image");
			return nullptr;
		}
		tex->Name      = name;
		tex->Filepath  = filename;
		tex->Width     = static_cast<Math::U32>(width);
		tex->Height    = static_cast<Math::U32>(height);
		tex->BindFlags = D3D11_BIND_SHADER_RESOURCE;
		tex->TexFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		tex->SrvFormat = DXGI_FORMAT_R8G8B8A8_UNORM;


		PRX_ASSERT_MSG(!tex->CreateTexture(imageData, width * 4), "Failed to create D3D texture2D from loaded image file");
		PRX_ASSERT_MSG(!tex->CreateSRV(), "Failed to create SRV from laoded image file");

		stbi_image_free(imageData);
		
		loadTimer.Stop();
		PRX_LOG_INFO("Loaded image file [%s] in %.2fms", filename, loadTimer.TotalTime() * 1000.0f);
		
		return tex;
	}
}