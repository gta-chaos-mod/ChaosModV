#pragma once

#include <random>

class Random
{
private:
	std::random_device m_RandomDevice;
	std::mt19937 m_Random {m_RandomDevice()};

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

	inline float GetRandomFloat(float fLower, float fUpper) {
		std::uniform_real_distribution<float> distr(fLower, fUpper);
		return distr(m_Random);
	}
};

inline Random g_Random;
