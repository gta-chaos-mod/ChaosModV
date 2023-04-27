#pragma once

struct Color
{
	unsigned int R, G, B, A;
};

// Returns a random RGBA value without a random alpha: 255 or max.
inline Color GetRandomColorRGB(unsigned int min = 0, unsigned int max = 255)
{
	return {
		(unsigned int)g_Random.GetRandomInt(min, max), // R
		(unsigned int)g_Random.GetRandomInt(min, max), // G
		(unsigned int)g_Random.GetRandomInt(min, max), // B
		255                                            // A
	};
}

// Returns a random RGBA value with a random alpha.
inline Color GetRandomColorRGBA(unsigned int min = 0, unsigned int max = 255)
{
	return {
		(unsigned int)g_Random.GetRandomInt(min, max), // R
		(unsigned int)g_Random.GetRandomInt(min, max), // G
		(unsigned int)g_Random.GetRandomInt(min, max), // B
		(unsigned int)g_Random.GetRandomInt(min, max)  // A
	};
}