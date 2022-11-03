#pragma once
#include <string>
#include <filesystem>
#include <fstream>

namespace Proximity::Utils
{
	enum class SpecialFolder
	{
		PROGRAM_FILES,
		APPDATA,
		LOCAL_APP_DATA,
		USER_PROFILE
	};

#define ASSETS_FOLDER_NAME "Assets"
#define SCRIPTS_FOLDER_NAME "Scripts"

	namespace fs = std::filesystem;
	class DirectoryManager
	{
	public:
		// Set the project working directory
		static void SetWorkingDirectory(const fs::path workingDirectory) noexcept;

		// Creates project dependent directories
		static void CreateProject();
		// Sets working directory and then creates project
		static void CreateProjectDir(const fs::path projectPath);
		// Clears project from root directory
		static void DeleteProjectDir();
		// Checks if all project dependent directories exist
		static bool ProjectDirExists();

		// Creates (sub) folders till target directory
		static void CreateDir(const fs::path dirPath);
		// Deletes specified directory
		static void DeleteDir(const fs::path dirPath);

		// Checks if a file exists
		static bool FileExists(const fs::path file);
		// Check if a directory exists
		static bool DirExists(const fs::path dir);

		static const fs::path& GetWorkingDir() noexcept;
		static const fs::path& GetAssetDir() noexcept;
		static const fs::path& GetScriptDir() noexcept;

		// True if project or dependent directories are not set up properly
		// Call CreateProject() or CreateProjectDir() to fix
		static bool s_ProjectDirIsDirty;

	private:
		static fs::path s_workingDirectory;
		static fs::path s_assetDirectory;
		static fs::path s_scriptsDirectory;
	};
}