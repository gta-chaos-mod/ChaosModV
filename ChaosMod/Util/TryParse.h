#pragma once

#include "Color.h"

#include <cstdlib>
#include <string_view>
#include <type_traits>

namespace Util
{
	template <typename T> bool TryParse(std::string_view text, T &result, int radix = 10);

	inline Color ParseConfigColorString(const std::string &colorText, bool &success)
	{
		// Format: #ARGB

		success = false;

		if (colorText.length() != 9)
			return Color();

		std::array<BYTE, 4> colors;

		int j = 0;
		for (int i = 1; i < 9; i += 2)
			if (!Util::TryParse<BYTE>(colorText.substr(i, 2), colors[j++], 16))
				return Color();

		success = true;
		return Color(colors[1], colors[2], colors[3], colors[0]);
	}

	template <typename T> inline bool TryParse(std::string_view text, T &result, int radix)
	{
		bool success = false;

		if constexpr (std::is_same<T, float>())
		{
			char *end;
			float parseResult = std::strtof(text.data(), &end);
			if (*end == '\0')
			{
				result  = parseResult;
				success = true;
			}
		}
		else if constexpr (std::is_same<T, Color>())
		{
			result = ParseConfigColorString(text.data(), success);
		}
		else
		{
			char *end;
			long parseResult;
			parseResult = std::strtol(text.data(), &end, radix);
			if (*end == '\0')
			{
				success = true;
				result  = static_cast<T>(parseResult);
			}
		}

		return success;
	}
}