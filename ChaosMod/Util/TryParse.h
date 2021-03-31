#pragma once

#include <cstdlib>

template <typename T>
inline bool TryParse(const std::string& text, T& result, int radix = 10)
{
	char* end;

	long parseResult;
	if constexpr (std::is_same<T, float>())
	{
		parseResult = std::strtof(text.c_str(), &end);
	}
	else
	{
		parseResult = std::strtol(text.c_str(), &end, radix);
	}
	
	bool parsed = *end == '\0';
	if (parsed)
	{
		result = parseResult;
	}
	
	return parsed;
}