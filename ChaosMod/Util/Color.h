#pragma once

#include <cstdint>

struct Color
{
	std::uint8_t R, G, B, A;

	std::uint32_t Get() const
	{
		return A << 24 | R << 16 | G << 8 | B;
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