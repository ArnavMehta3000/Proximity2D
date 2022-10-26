#include "Utils/Logger.h"
#include <Windows.h>
#include <ctime>

namespace Proximity::Utils
{
	LogLevel    Logger::s_logLevel = LOG_LEVEL_DEBUG;
	std::mutex  Logger::s_logMutex;
	bool        Logger::s_outputToFile = false;
	FILE*       Logger::s_file = 0;
	const char* Logger::s_filePath = 0;

	void Logger::Init(LogLevel level)
	{
		s_logLevel = level;

		AllocConsole();
		AttachConsole(GetCurrentProcessId());
		FILE* fp = nullptr;
		freopen_s(&fp, "CONIN$", "r", stdin);
		freopen_s(&fp, "CONOUT$", "w", stdout);
		freopen_s(&fp, "CONOUT$", "w", stderr);
	}

	void Logger::Shutdown()
	{
		FreeFile();
	}

	void Logger::SetFileOutput()
	{
		s_outputToFile = true;
		s_filePath = "Logs/PrxLog.txt";
		EnablefileOutput();
	}

	void Logger::SetFileOutput(const char* filePath)
	{
		s_outputToFile = true;
		s_filePath = filePath;
		EnablefileOutput();
	}

	void Logger::EnablefileOutput()
	{
		if (s_file != 0)
			FreeFile();  // Close any previously opened files

		fopen_s(&s_file, s_filePath, "a");  // Append

		if (s_file == 0)
		{
			std::string s("Logger: Failed to open file at: ");
			OutputDebugStringA((s + s_filePath).c_str());
		}
	}

	void Logger::FreeFile()
	{
		fclose(s_file);
		s_file = 0;
	}

	std::string Logger::GetFormattedTime()
	{
		std::time_t currentTime = std::time(0);
		std::tm timestamp;
		localtime_s(&timestamp, &currentTime);

		char buffer[50];
		strftime(buffer, 50, "%c", &timestamp);

		return std::string(buffer);
	}
}