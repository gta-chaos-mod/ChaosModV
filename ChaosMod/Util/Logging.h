#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

inline std::ofstream g_Log("chaosmod/chaoslog.txt");

inline std::ofstream g_ConsoleOut;

inline const auto g_ModStartTime = std::time(nullptr);

#define _OSS std::ostringstream()

#define __FILENAME__                                        \
	(strrchr(__FILE__, '\\')  ? strrchr(__FILE__, '\\') + 1 \
	 : strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1  \
	                          : __FILE__)

#define _LOG(_text, _stream) _stream << _text

#define RAW_LOG(_text)          \
	do                          \
	{                           \
		_LOG(_text, g_Log);     \
		_LOG(_text, std::cout); \
	} while (0)

#define COLOR_PREFIX_LOG(_prefix, _text)                                                                              \
	do                                                                                                                \
	{                                                                                                                 \
		auto curTime      = std::time(nullptr);                                                                       \
		auto diffTime     = static_cast<time_t>(std::difftime(curTime, g_ModStartTime));                              \
		auto diffTimeTime = *std::gmtime(&diffTime);                                                                  \
		_LOG("[" << std::put_time(&diffTimeTime, "%H:%M:%S") << "] " << _prefix << " " << _text << std::endl, g_Log); \
		if (GetConsoleWindow())                                                                                       \
		{                                                                                                             \
			_LOG("\033[" << 90 + (std::hash<std::string> {}((std::stringstream() << _prefix).str()) % 6) << "m"       \
			             << _prefix << "\033[0m " << _text << std::endl,                                              \
			     std::cout);                                                                                          \
		}                                                                                                             \
	} while (0)

#define LOG(_text) COLOR_PREFIX_LOG("(" << __FILENAME__ << ")", _text);

#ifdef _DEBUG
#define DEBUG_LOG(_text) LOG(_text)
#else
#define DEBUG_LOG(_text)
#endif