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
			auto name = DirectoryManager::GetFileNameFromDir(dir.path(), true);
			auto ptr = std::make_shared<Scripting::LuaScript>(dir.path().string());
			AddLuaScript(ptr);
		}
	}
	void ScriptLibrary::Refresh()
	{
		bool found = false;
		PRX_LOG_DEBUG("Refreshing audio library for audio");

		auto& rootPath = DirectoryManager::s_appDirectories.AudioPath;
		using recursiveDirIter = std::filesystem::recursive_directory_iterator;
		
		for (const auto& dir : recursiveDirIter(rootPath))
		{
			auto name = DirectoryManager::GetFileNameFromDir(dir.path(), true);
			if (!Exists(name))
			{
				if (dir.path().extension() == ".lua")
				{
					auto ptr = std::make_shared<Scripting::LuaScript>(dir.path().string());
					AddLuaScript(ptr);
				}
				found = true;
			}
			else
			{
				// No new image files
			}
		}

		if (found)
			PRX_LOG_DEBUG("Refreshed script library - added/removed script files");
		else
			PRX_LOG_DEBUG("Refreshed script library - no changes");
	}

	void ScriptLibrary::AddLuaScript(const std::shared_ptr<Scripting::LuaScript>& src)
	{
		auto& name = src->GetName();
		if (Exists(name))
		{
			PRX_LOG_WARN("Script file with the same name [%s] already exists in audio library", name.c_str());
			return;
		}
		m_scripts[name] = src;
	}

	void ScriptLibrary::RemoveLuaScript(const std::string& scriptName)
	{
		if (m_scripts.erase(scriptName) == 1)
			PRX_LOG_INFO("Successfully removed script [%s] from library", scriptName.c_str());
		else
			PRX_LOG_INFO("Failed to remove script [%s] from library", scriptName.c_str());
	}

	bool ScriptLibrary::Exists(const std::string& scriptName)
	{
		return m_scripts.contains(scriptName);
	}

	std::shared_ptr<Scripting::LuaScript> ScriptLibrary::Get(const std::string& scriptName)
	{
		if (!Exists(scriptName))
		{
			PRX_LOG_ERROR("Script [%s] not found", scriptName.c_str());
			return nullptr;
		}
		else
			return m_scripts[scriptName];
	}
}