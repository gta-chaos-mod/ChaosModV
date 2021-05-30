#pragma once

#include <cstdlib>

namespace Util
{
	template <typename T>
	inline bool TryParse(const std::string& szText, T& result, int iRadix = 10)
	{
		char* cEnd;

		long lParseResult;

		if constexpr (std::is_same<T, float>())
		{
			lParseResult = std::strtof(szText.c_str(), &cEnd);
		}
		else
		{
			lParseResult = std::strtol(szText.c_str(), &cEnd, iRadix);
		}

		bool bParsed = *cEnd == '\0';
		if (bParsed)
		{
			result = static_cast<T>(lParseResult);
		}

		return bParsed;
	}
}