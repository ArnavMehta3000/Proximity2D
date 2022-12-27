#pragma once
#include <string>
#include <filesystem>
#include <fstream>

#undef MoveFile
#undef DeleteFile
#undef CreateFile

namespace Proximity::Utils
{
	using FilePath = std::filesystem::path;

	struct AppDirectories
	{
		FilePath RootPath;
		FilePath ShadersPath;
		FilePath ScriptsPath;
		FilePath TexturesPath;
		FilePath AudioPath;
	};

	class DirectoryManager
	{
	public:
		static void Init(HWND handle);
		static bool CreateDir(const FilePath& dir);
		static bool DeleteDir(const FilePath& dir);
		static void RenameDir(const FilePath& oldName, const std::string& newName);
		static bool CreateFile(const FilePath& dir);
		static void MoveDir(const FilePath& oldDir, const FilePath& newDir);
		static void MoveFile(const FilePath& srcDir, const FilePath& dstDir);
		static bool DeleteFile(const FilePath& dir);
		static std::string GetFileNameFromDir(const FilePath& dir, bool includeExtension = true);
		static bool Exists(const FilePath& path);
		static Math::U32 GetSize(const FilePath& filepath);
		static bool ShowInExplorer(const FilePath& path);

		// These return empty string if cancelled
		static std::string OpenFileFromExplorer(const char* filter);
		static std::string OpenDirFromExplorer(const char* filter);
		static std::string SaveFileFromExplorer(const char* filter);


		static AppDirectories s_appDirectories;
	};
}