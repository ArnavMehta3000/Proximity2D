#include "Utils/DirectoryManager.h"
#include "Utils/Logger.h"

namespace Proximity::Utils
{
	fs::path DirectoryManager::s_workingDirectory;
	fs::path DirectoryManager::s_assetDirectory;
	fs::path DirectoryManager::s_scriptsDirectory;
	bool     DirectoryManager::s_ProjectDirIsDirty = true;
	
	void DirectoryManager::SetWorkingDirectory(const fs::path workingDirectory) noexcept
	{
		s_workingDirectory = workingDirectory;
		// TODO: Create asset and file directories
		s_ProjectDirIsDirty = false;
	}

	void DirectoryManager::CreateProject()
	{
		if (ProjectDirExists())
			return;

		// Create scripts directory
		s_scriptsDirectory = s_workingDirectory;
		s_scriptsDirectory.append(SCRIPTS_FOLDER_NAME);
		CreateDir(s_scriptsDirectory);

		// Create assets directory
		s_assetDirectory = s_workingDirectory;
		s_assetDirectory.append(ASSETS_FOLDER_NAME);
		CreateDir(s_assetDirectory);
	}

	void DirectoryManager::CreateProjectDir(const fs::path projectPath)
	{
		SetWorkingDirectory(projectPath);
		CreateProject();
	}

	void DirectoryManager::DeleteProjectDir()
	{
		DeleteDir(s_workingDirectory);
		s_workingDirectory.clear();
		s_assetDirectory.clear();
		s_scriptsDirectory.clear();
		s_ProjectDirIsDirty = true;
	}

	bool DirectoryManager::ProjectDirExists()
	{
		fs::path tempPath = s_workingDirectory;
		bool flag = true;  // false if any of the sub directories don't exist
		
		// Check for scripts directory
		tempPath.append(SCRIPTS_FOLDER_NAME);
		if (!DirExists(tempPath))
		{
			s_ProjectDirIsDirty = true;
			flag = false;
		}

		tempPath.clear();
		tempPath = s_workingDirectory;

		// Check for assets directory
		tempPath.append(ASSETS_FOLDER_NAME);
		if (!DirExists(tempPath))
		{
			s_ProjectDirIsDirty = true;
			flag = false;
		}

		return flag;
	}

	void DirectoryManager::CreateDir(const fs::path dirPath)
	{
		if (!fs::create_directories(dirPath))
			PRX_LOG_ERROR("Failed to create directory %s", dirPath.c_str());
	}

	void DirectoryManager::DeleteDir(const fs::path dirPath)
	{
		std::error_code ec;
		if (!fs::remove_all(dirPath, ec))
			PRX_LOG_ERROR("Failed to delete directory Error: &s", ec.message().c_str());
	}

	bool DirectoryManager::FileExists(const fs::path file)
	{
		return fs::exists(file);
	}

	bool DirectoryManager::DirExists(const fs::path dir)
	{
		return fs::exists(dir);
	}


	const fs::path& DirectoryManager::GetWorkingDir() noexcept
	{
		return s_workingDirectory;
	}

	const fs::path& DirectoryManager::GetAssetDir() noexcept
	{
		return s_assetDirectory;
	}

	const fs::path& DirectoryManager::GetScriptDir() noexcept
	{
		return s_scriptsDirectory;
	}
}
