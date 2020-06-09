#pragma once

#include <cstdlib>

inline bool TryParseInt(const char* text, int& result, int radix = 10)
{
	char* end;
	long parseResult = std::strtol(text, &end, radix);
	
	bool parsed = *end == '\0';
	if (parsed)
	{
		result = parseResult;
	}
	
	return parsed;
}

inline bool TryParseInt(const std::string& text, int& result, int radix = 10)
{
	return TryParseInt(text.c_str(), result, radix);
}