#pragma once
#include "Engine/Systems/UUID.h"
namespace Proximity::Core
{
	// UUID for assets
	using AssetHandle = UUID;

	enum class AssetType
	{
		UNKNOWN,
		TEXTURE,
		SHADER,
		AUDIO
	};

	struct AssetInfo
	{
		AssetHandle m_Handle;
		AssetType   m_Type;
		std::string m_FullPath;
		// TODO: add other reference to asset paths here
		// Soure path
		// Import data
		// Modification date
	};
}