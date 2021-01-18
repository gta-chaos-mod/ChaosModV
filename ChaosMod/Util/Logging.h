#pragma once

#include <sstream>
#include <fstream>

inline std::ofstream g_log("chaosmod/chaoslog.txt");

#define _OSS std::ostringstream()

#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)

// put text inside ostringstream so modifiers don't affect ofstream
#define LOG(_text) g_log << "[" << __FILENAME__ << "] " << __FUNCTION__ << ": " << (std::ostringstream() << _text).str() << std::endl

#ifdef _DEBUG
#define DEBUG_LOG(_text) LOG(_text)
#else
#define DEBUG_LOG(_text)
#endif