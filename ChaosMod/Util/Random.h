#pragma once

#include <random>

class Random
{
  private:
	std::random_device m_RandomDevice;
	std::mt19937 m_Random { m_RandomDevice() };

  public:
	inline void SetSeed(int seed)
	{
		if (seed > 0)
		{
			m_Random.seed(seed);
		}
	}

	inline int GetRandomInt(int lower, int upper)
	{
		std::uniform_int_distribution<> distr(lower, upper);
		return distr(m_Random);
	}

	inline float GetRandomFloat(float lower, float upper)
	{
		std::uniform_real_distribution<float> distr(lower, upper);
		return distr(m_Random);
	}
};

inline Random g_Random;
