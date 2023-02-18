#include "enginepch.h"
#include "Utils/Logger.h"
#include "Utils/DirectoryManager.h"
#include "Utils/Assertions.h"
#include <Windows.h>
#include <ctime>

namespace Proximity::Utils
{
	LogLevel              Logger::s_logLevel = LOG_LEVEL_DEBUG;
	std::mutex            Logger::s_logMutex;
	bool                  Logger::s_outputToFile = false;
	FILE*                 Logger::s_file = 0;
	std::filesystem::path Logger::s_filePath;

	void Logger::Init(LogLevel level)
	{
		s_logLevel = level;
		s_filePath = "Log.prx";

		AllocConsole();
		AttachConsole(GetCurrentProcessId());
		FILE* fp = nullptr;
		freopen_s(&fp, "CONIN$", "r", stdin);
		freopen_s(&fp, "CONOUT$", "w", stdout);
		freopen_s(&fp, "CONOUT$", "w", stderr);
	}

	void Logger::Shutdown()
	{
		if (s_file == 0)
			return;

		fprintf_s(s_file, "\n");
		FreeFile();
	}

	void Logger::SetFileOutput()
	{
		s_outputToFile = true;
		s_filePath = DirectoryManager::s_appDirectories.RootPath / "EngineLog.prx";

		EnablefileOutput();
	}

	void Logger::UpdateFile()
	{
		FreeFile();
		EnablefileOutput();
	}

	void Logger::LogToBuffer(const std::string log)
	{
		Proximity::Core::Globals::g_engineDebugBuffer->AddToStream(log);
	}

	void Logger::EnablefileOutput()
	{
		if (s_file != 0)
			FreeFile();  // Close any previously opened files

		fopen_s(&s_file, s_filePath.string().c_str(), "a");  // Append

		if (s_file == 0)
		{
			std::string s("Logger: Failed to open file at: ");
			OutputDebugStringA((s + s_filePath.string()).c_str());
		}
		else
		{
			PRX_LOG_INFO("LOG FILE FOUND/CREATED AFTR ENGINE INITIALIZATION");
			PRX_LOG_INFO("LOG FILE PATH: %s", s_filePath.string().c_str());
		}
	}

	void Logger::FreeFile()
	{
		if (s_file == 0)
			return;

		fclose(s_file);
		s_file = 0;
	}

	std::string Logger::GetFormattedTime()
	{
		std::time_t currentTime = std::time(0);
		std::tm timestamp;
		localtime_s(&timestamp, &currentTime);

		char buffer[100];
		strftime(buffer, 100, "[%b-%d] [%r]", &timestamp);

		return std::string(buffer);
	}

	void ReportAssertionFailure(const char* expression, const char* message)
	{
		PRX_LOG_FATAL("{ASSERTION} Expression: %s Message: %s", expression, message);
	}
}