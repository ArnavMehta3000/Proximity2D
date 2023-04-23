#include "enginepch.h"
#include "Engine/Modules/ScriptLibrary.h"

namespace Proximity::Modules
{
	void ScriptLibrary::InitProjectLib()
	{
		auto& rootPath = DirectoryManager::s_appDirectories.ScriptsPath;
		using recursiveDirIter = std::filesystem::recursive_directory_iterator;

		for (const auto& dir : recursiveDirIter(rootPath))
		{
			// Save full path to script
			m_scripts.push_back(dir.path().string());
		}
	}
}