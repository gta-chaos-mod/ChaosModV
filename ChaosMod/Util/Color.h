#pragma once

#include "TryParse.h"

#include <cstdint>

struct Color;
namespace Util
{
	Color ParseConfigColorString(const std::string &colorText, bool &success);
}

struct Color
{
	std::uint8_t R, G, B, A;

	inline std::uint32_t Get() const
	{
		return A << 24 | R << 16 | G << 8 | B;
	}

	Color(std::string colorString)
	{
		bool _;
		*this = Util::ParseConfigColorString(colorString, _);
	}

	Color() : R(0), G(0), B(0), A(0)
	{
	}

	Color(std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a) : R(r), G(g), B(b), A(a)
	{
	}

	Color(std::uint8_t r, std::uint8_t g, std::uint8_t b) : Color(r, g, b, 255)
	{
	}
};

// Returns a random RGBA value without a random alpha: 255 or max.
inline Color GetRandomColorRGB(std::uint8_t min = 0, std::uint8_t max = 255)
{
	return {
		(std::uint8_t)g_RandomNoDeterm.GetRandomInt(min, max), // R
		(std::uint8_t)g_RandomNoDeterm.GetRandomInt(min, max), // G
		(std::uint8_t)g_RandomNoDeterm.GetRandomInt(min, max), // B
		255                                                    // A
	};
}

// Returns a random RGBA value with a random alpha.
inline Color GetRandomColorRGBA(std::uint8_t min = 0, std::uint8_t max = 255)
{
	return {
		(std::uint8_t)g_RandomNoDeterm.GetRandomInt(min, max), // R
		(std::uint8_t)g_RandomNoDeterm.GetRandomInt(min, max), // G
		(std::uint8_t)g_RandomNoDeterm.GetRandomInt(min, max), // B
		(std::uint8_t)g_RandomNoDeterm.GetRandomInt(min, max)  // A
	};
}