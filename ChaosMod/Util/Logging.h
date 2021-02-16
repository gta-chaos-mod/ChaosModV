#pragma once

#include <sstream>
#include <fstream>
#include <iostream>

inline std::ofstream g_log("chaosmod/chaoslog.txt");

inline std::ofstream g_consoleOut;

#define _OSS std::ostringstream()

#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)

#define _LOG(_text, _stream) _stream << _text << std::endl

#define RAW_LOG(_text) do { _LOG(_text, g_log); _LOG(_text, std::cout); } while (0)
#define LOG(_text) RAW_LOG("[" << __FILENAME__ << "] " << __FUNCTION__ << ": " << (std::ostringstream() << _text).str())

#ifdef _DEBUG
#define DEBUG_LOG(_text) LOG(_text)
#else
#define DEBUG_LOG(_text)
#endif