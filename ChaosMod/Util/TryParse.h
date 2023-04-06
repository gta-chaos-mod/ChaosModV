#pragma once

#include <cstdlib>
#include <string_view>
#include <type_traits>

namespace Util
{
	template <typename T> inline bool TryParse(std::string_view text, T &result, int radix = 10)
	{
		char *cEnd;

		long lParseResult;

		if constexpr (std::is_same<T, float>())
		{
			lParseResult = std::strtof(text.data(), &cEnd);
		}
		else
		{
			lParseResult = std::strtol(text.data(), &cEnd, radix);
		}

		bool parsed = *cEnd == '\0';
		if (parsed)
		{
			result = static_cast<T>(lParseResult);
		}

		return parsed;
	}
}