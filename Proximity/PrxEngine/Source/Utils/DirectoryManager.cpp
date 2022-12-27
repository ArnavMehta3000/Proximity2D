#include "enginepch.h"
#include "Utils/DirectoryManager.h"
#include "Utils/Logger.h"
#include <commdlg.h>
#include <Shlobj.h>

namespace Proximity::Utils
{
	namespace fs = std::filesystem;

	static HWND s_window = NULL;
	AppDirectories DirectoryManager::s_appDirectories;

	void DirectoryManager::Init(HWND handle)
	{
		s_window = handle;
	}

	bool DirectoryManager::CreateDir(const FilePath& dir)
	{
		return fs::create_directories(dir);
	}

	bool DirectoryManager::DeleteDir(const FilePath& dir)
	{
		return fs::remove_all(dir);
	}

	void DirectoryManager::RenameDir(const FilePath& oldName, const std::string& newName)
	{
		fs::rename(oldName, newName);
	}

	bool DirectoryManager::CreateFile(const FilePath& dir)
	{
		return false;
	}

	void DirectoryManager::MoveDir(const FilePath& oldDir, const FilePath& newDir)
	{
		auto copyOptions = std::filesystem::copy_options::update_existing | std::filesystem::copy_options::recursive;

		FilePath path = newDir / oldDir.filename();  // '/' Appends paths together
		CreateDir(path);
		fs::copy(oldDir, path, copyOptions);
		DeleteDir(oldDir);
	}

	void DirectoryManager::MoveFile(const FilePath& srcDir, const FilePath& dstDir)
	{
		auto copyOptions = std::filesystem::copy_options::update_existing | std::filesystem::copy_options::recursive;
		fs::copy(srcDir, dstDir, copyOptions);
		DirectoryManager::DeleteFile(srcDir);
	}

	bool DirectoryManager::DeleteFile(const FilePath& dir)
	{
		return fs::remove(dir);
	}

	std::string DirectoryManager::GetFileNameFromDir(const FilePath& dir, bool includeExtension)
	{
		return (includeExtension) ? dir.filename().string() : dir.stem().string();
	}

	bool DirectoryManager::Exists(const FilePath& path)
	{
		return fs::exists(path);
	}

	Math::U32 DirectoryManager::GetSize(const FilePath& filepath)
	{
		return (Math::U32)fs::file_size(filepath);
	}

	bool DirectoryManager::ShowInExplorer(const FilePath& path)
	{
		// TODO: implement this
		return false;
	}

	std::string DirectoryManager::OpenFileFromExplorer(const char* filter)
	{
		CREATE_ZERO(OPENFILENAMEA, ofn);
		CHAR szFile[260] = { 0 };

		ofn.lStructSize  = sizeof(OPENFILENAMEA);
		ofn.hwndOwner    = s_window;
		ofn.lpstrFile    = szFile;
		ofn.nMaxFile     = sizeof(szFile);
		ofn.lpstrFilter  = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags        = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

		if (GetOpenFileNameA(&ofn) == TRUE)
			return ofn.lpstrFile;
		else
			return std::string();
	}

	std::string DirectoryManager::OpenDirFromExplorer(const char* title)
	{
		CHAR szDir[260] = { 0 };
		CREATE_ZERO(BROWSEINFOA, bInfo);
		bInfo.hwndOwner      = s_window;
		bInfo.pidlRoot       = NULL;
		bInfo.pszDisplayName = szDir; // Address of a buffer to receive the display name of the folder selected by the user
		bInfo.lpszTitle      = "Please, select a folder"; // Title of the dialog
		bInfo.ulFlags        = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
		bInfo.lpfn           = NULL;
		bInfo.lParam         = 0;
		bInfo.iImage         = -1;

		LPITEMIDLIST lpItem = SHBrowseForFolderA(&bInfo);
		if (lpItem != NULL)
		{
			SHGetPathFromIDListA(lpItem, szDir);
			return szDir;
		}
		else
			return std::string();
	}

	std::string DirectoryManager::SaveFileFromExplorer(const char* filter)
	{
		CREATE_ZERO(OPENFILENAMEA, ofn);
		CHAR szFile[260] = { 0 };

		ofn.lStructSize   = sizeof(OPENFILENAMEA);
		ofn.hwndOwner     = s_window;
		ofn.lpstrFile     = szFile;
		ofn.nMaxFile      = sizeof(szFile);
		ofn.lpstrFilter   = filter;
		ofn.nFilterIndex  = 1;
		ofn.Flags         = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

		if (GetSaveFileNameA(&ofn) == TRUE)
			return ofn.lpstrFile;
		else
			return std::string();
	}


}
