#pragma once
#include <stdio.h>
#include <mutex>
#include <filesystem>
#include "Utils/TextBuffer.h"

namespace Proximity::Utils
{
	enum LogLevel
	{
		LOG_LEVEL_FATAL = 0,  // Engine crashing level failure
		LOG_LEVEL_ERROR = 1,
		LOG_LEVEL_WARN  = 2,
		LOG_LEVEL_INFO  = 3,
		LOG_LEVEL_DEBUG = 4,
	};

	// Thread safe static logging class
	class Logger
	{
	public:
		static void Init(LogLevel level = LOG_LEVEL_DEBUG);
		static void Shutdown();

		static void SetFileOutput();
		static void SetFileOutput(bool set) { s_outputToFile = set; }



		template<typename... Args>
		static void Debug(int line, const char* filename, const char* msg, Args... args)
		{
			Log(line, filename, LOG_LEVEL_DEBUG, msg, args...);
		}

		template<typename... Args>
		static void Info(int line, const char* filename, const char* msg, Args... args)
		{
			Log(line, filename, LOG_LEVEL_INFO, msg, args...);
		}

		template<typename... Args>
		static void Warn(int line, const char* filename, const char* msg, Args... args)
		{
			Log(line, filename, LOG_LEVEL_WARN, msg, args...);
		}

		template<typename... Args>
		static void Error(int line, const char* filename, const char* msg, Args... args)
		{
			Log(line, filename, LOG_LEVEL_ERROR, msg, args...);
		}

		template<typename... Args>
		static void Fatal(int line, const char* filename, const char* msg, Args... args)
		{
			Log(line, filename, LOG_LEVEL_FATAL, msg, args...);
		}


		template<typename... Args>
		static void Debug(const char* msg, Args... args)
		{
			Log(LOG_LEVEL_DEBUG, msg, args...);
		}

		template<typename... Args>
		static void Info(const char* msg, Args... args)
		{
			Log(LOG_LEVEL_INFO, msg, args...);
		}

		template<typename... Args>
		static void Warn(const char* msg, Args... args)
		{
			Log(LOG_LEVEL_WARN, msg, args...);
		}

		template<typename... Args>
		static void Error(const char* msg, Args... args)
		{
			Log(LOG_LEVEL_ERROR, msg, args...);
		}

		template<typename... Args>
		static void Fatal(const char* msg, Args... args)
		{
			Log(LOG_LEVEL_FATAL, msg, args...);
		}

		static void UpdateFile();

	private:
		template <typename... Args>
		static void Log(LogLevel level, const char* msg, Args... args)
		{
			std::scoped_lock lock(s_logMutex);
			const char* levelStr[] = { " [FATAL]: " , " [ERROR]: " , " [WARN]: " , " [INFO]: " , " [DEBUG]: " };

#ifdef _DEBUG  // Only print to console if in debug release
			if (s_logLevel >= level && level >= 0)
			{
				printf_s(GetFormattedTime().c_str());
				printf_s(levelStr[level]);
				printf_s(msg, args...);
				printf_s("\n");
			}
#endif // _DEBUG


			// Check for logging to file
			if (s_file && !s_filePath.empty())
			{
				fprintf_s(s_file, GetFormattedTime().c_str());
				fprintf_s(s_file, levelStr[level]);
				fprintf_s(s_file, msg, args...);
				fprintf_s(s_file, "\n");
			}
			std::string s = GetFormattedTime().append(levelStr[level]).append(Format(msg, args...));
			LogToBuffer(s);
		}

		template <typename... Args>
		static void Log(int line, const char* filename, LogLevel level, const char* msg, Args... args)
		{
			std::scoped_lock lock(s_logMutex);
			const char* levelStr[] = { " [FATAL]: " , " [ERROR]: " , " [WARN]: " , " [INFO]: " , " [DEBUG]: " };

#ifdef _DEBUG
			if (s_logLevel >= level && level >= 0)
			{
				printf_s(GetFormattedTime().c_str());
				printf_s(levelStr[level]);
				printf_s(msg, args...);
				printf_s(" on line %d in %s", line, filename);
				printf_s("\n");
			}
#endif // _DEBUG


			// Check for logging to file
			if (s_file && !s_filePath.empty())
			{
				fprintf_s(s_file, GetFormattedTime().c_str());
				fprintf_s(s_file, levelStr[level]);
				fprintf_s(s_file, msg, args...);
				fprintf_s(s_file, " LINE: %d in %s", line, filename);
				fprintf_s(s_file, "\n");

			}
			std::string s = GetFormattedTime().append(levelStr[level]).append(Format(msg, args...));
			LogToBuffer(s);
		}

		template <typename... Args>
		static std::string Format(const char* msg, Args... args)
		{
			char buf[1024];
			snprintf(buf, sizeof(buf), msg, args...);
			return std::string(buf);
		}

		static void LogToBuffer(const std::string log);
		static void EnablefileOutput();
		static void FreeFile();
		static std::string GetFormattedTime();

	private:
		static LogLevel    s_logLevel;
		static std::mutex  s_logMutex;

		static std::filesystem::path s_filePath;
		static bool        s_outputToFile;
		static FILE*       s_file;
	};


#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)

#ifdef _DEBUG
#define PRX_LOG_DEBUG(msg, ...) Proximity::Utils::Logger::Debug(__LINE__, __FILENAME__, msg, __VA_ARGS__)
#define PRX_LOG_INFO(msg, ...) Proximity::Utils::Logger::Info(__LINE__, __FILENAME__, msg, __VA_ARGS__)
#define PRX_LOG_WARN(msg, ...) Proximity::Utils::Logger::Warn(__LINE__, __FILENAME__, msg, __VA_ARGS__)
#define PRX_LOG_ERROR(msg, ...) Proximity::Utils::Logger::Error(__LINE__, __FILENAME__, msg, __VA_ARGS__)
#define PRX_LOG_FATAL(msg, ...) Proximity::Utils::Logger::Fatal(__LINE__, __FILENAME__, msg, __VA_ARGS__)  

#else

#define PRX_LOG_DEBUG(msg, ...) Proximity::Utils::Logger::Debug(msg, __VA_ARGS__)
#define PRX_LOG_INFO(msg, ...) Proximity::Utils::Logger::Info(msg, __VA_ARGS__)
#define PRX_LOG_WARN(msg, ...) Proximity::Utils::Logger::Warn(msg, __VA_ARGS__)
#define PRX_LOG_ERROR(msg, ...) Proximity::Utils::Logger::Error(msg, __VA_ARGS__)
#define PRX_LOG_FATAL(msg, ...) Proximity::Utils::Logger::Fatal(msg, __VA_ARGS__)
#endif // _DEBUG
}