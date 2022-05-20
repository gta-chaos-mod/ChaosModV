#pragma once

#include <random>

//#include "Colour.h"

class Random
{
  private:
	std::random_device m_RandomDevice;
	std::mt19937 m_Random { m_RandomDevice() };

  public:
	inline void SetSeed(int iSeed)
	{
		if (iSeed > 0)
		{
			m_Random.seed(iSeed);
		}
	}

	inline int GetRandomInt(int iLower, int iUpper)
	{
		std::uniform_int_distribution<> distr(iLower, iUpper);
		return distr(m_Random);
	}

	inline float GetRandomFloat(float fLower, float fUpper)
	{
		std::uniform_real_distribution<float> distr(fLower, fUpper);
		return distr(m_Random);
	}

	// Returns a random RGBA value without a random alpha: 255 or max.
	inline CRGBA GetRandomColourRGB(unsigned int min = 0, unsigned int max = 255)
	{
		return { (unsigned int)GetRandomInt(min, max), (unsigned int)GetRandomInt(min, max), (unsigned int)GetRandomInt(min, max), max };
	}

	// Returns a random RGBA value with a random alpha.
	inline CRGBA GetRandomColourRGBA(unsigned int min = 0, unsigned int max = 255)
	{
		return { (unsigned int)GetRandomInt(min, max), (unsigned int)GetRandomInt(min, max), (unsigned int)GetRandomInt(min, max), (unsigned int)GetRandomInt(min, max) };
	}
};

inline Random g_Random;
