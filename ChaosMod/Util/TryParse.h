#pragma once

#include <cstdlib>
#include <string_view>
#include <type_traits>

namespace Util
{
	template <typename T> inline bool TryParse(std::string_view szText, T &result, int iRadix = 10)
	{
		char *cEnd;

		long lParseResult;

		if constexpr (std::is_same<T, float>())
		{
			lParseResult = std::strtof(szText.data(), &cEnd);
		}
		else
		{
			lParseResult = std::strtol(szText.data(), &cEnd, iRadix);
		}

		bool bParsed = *cEnd == '\0';
		if (bParsed)
		{
			result = static_cast<T>(lParseResult);
		}

		return bParsed;
	}
}