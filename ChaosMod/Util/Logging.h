#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>

#define CHAOS_LOG_FILE "chaosmod/chaoslog.txt"

inline std::ofstream g_Log(CHAOS_LOG_FILE);

inline std::ofstream g_ConsoleOut;

inline const auto g_ModStartTime = std::time(nullptr);

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
			_LOG("\033[" << 90 + (std::hash<std::string> {}((std::ostringstream() << _prefix).str()) % 6) << "m"      \
			             << _prefix << "\033[0m " << _text << std::endl,                                              \
			     std::cout);                                                                                          \
		}                                                                                                             \
	} while (0)

#define LOG(_text)                                                                           \
	do                                                                                       \
	{                                                                                        \
		static const auto fileName = []() consteval                                          \
		{                                                                                    \
			std::string_view filePath(__FILE__);                                             \
			size_t slashPos = filePath.find_last_of("\\");                                   \
			auto fileName   = filePath.substr(slashPos != filePath.npos ? slashPos + 1 : 0); \
			slashPos        = fileName.find_last_of("/");                                    \
			fileName        = fileName.substr(slashPos != fileName.npos ? slashPos + 1 : 0); \
			return fileName;                                                                 \
		}                                                                                    \
		();                                                                                  \
		COLOR_PREFIX_LOG("(" << fileName << ")", _text);                                     \
	} while (0)

#ifdef _DEBUG
#define DEBUG_LOG(_text) LOG(_text)
#else
#define DEBUG_LOG(_text)
#endif