#pragma once

struct Color
{
	uint8_t R, G, B, A;

	uint32_t Get()
	{
		return A << 24 | R << 16 | G << 8 | B;
	}
};

// Returns a random RGBA value without a random alpha: 255 or max.
inline Color GetRandomColorRGB(uint8_t min = 0, uint8_t max = 255)
{
	return {
		(uint8_t)g_Random.GetRandomInt(min, max), // R
		(uint8_t)g_Random.GetRandomInt(min, max), // G
		(uint8_t)g_Random.GetRandomInt(min, max), // B
		255                                       // A
	};
}

// Returns a random RGBA value with a random alpha.
inline Color GetRandomColorRGBA(uint8_t min = 0, uint8_t max = 255)
{
	return {
		(uint8_t)g_Random.GetRandomInt(min, max), // R
		(uint8_t)g_Random.GetRandomInt(min, max), // G
		(uint8_t)g_Random.GetRandomInt(min, max), // B
		(uint8_t)g_Random.GetRandomInt(min, max)  // A
	};
}