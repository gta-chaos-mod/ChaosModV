#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

inline std::ofstream g_Log("chaosmod/chaoslog.txt");

inline std::ofstream g_ConsoleOut;

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

#define COLOR_PREFIX_LOG(_prefix, _text)                                                                         \
	do                                                                                                           \
	{                                                                                                            \
		if (!GetConsoleWindow())                                                                                 \
		{                                                                                                        \
			RAW_LOG(_prefix << " " << _text << std::endl);                                                       \
		}                                                                                                        \
		else                                                                                                     \
		{                                                                                                        \
			_LOG(_prefix << " " << _text << std::endl, g_Log);                                                   \
			_LOG("\033[" << 90 + (std::hash<std::string> {}((std::ostringstream() << _prefix).str()) % 6) << "m" \
			             << _prefix << "\033[0m " << _text << std::endl,                                         \
			     std::cout);                                                                                     \
		}                                                                                                        \
	} while (0)

#define LOG(_text) COLOR_PREFIX_LOG("[" << __FILENAME__ << "]", _text)

#ifdef _DEBUG
#define DEBUG_LOG(_text) LOG(_text)
#else
#define DEBUG_LOG(_text)
#endif