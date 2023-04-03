#pragma once
#include "Graphics/Rendering/RenderingStructures.h"

namespace Proximity::Modules
{
	class TextureLoader
	{
	public:
		static bool CreateBinaryFromImage(const std::string& imageFile, const std::string& outputFile);
		static bool CreateTextureFromBinary(Graphics::Texture2D* outputTex, const std::string& binFile);
		static bool GetImageSize(const std::string& filename, Math::U32* x, Math::U32* y);
	};
}