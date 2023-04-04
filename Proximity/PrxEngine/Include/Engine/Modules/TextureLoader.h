#pragma once
#include "Graphics/Rendering/RenderingStructures.h"

namespace Proximity::Modules
{
	class TextureLoader
	{
	public:
		// Creates a texture with a D3D-SRV of an image file
		static Graphics::Texture2D* Load(const char* filename, const std::string& name);
	};
}