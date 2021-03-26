#pragma once

#include <random>
#include <time.h>

class Random
{
public:
	inline void SetSeed(int seed)
	{
		if (seed > 0)
		{
			random.seed(seed);
		}
	}

	inline int GetRandomInt(int lower, int upper)
	{
		std::uniform_int_distribution<> distr(lower, upper);
		return distr(random);
	}

	inline float GetRandomFloat(float lower, float upper) {
		std::uniform_real_distribution<float> distr(lower, upper);
		return distr(random);
	}

private:
	std::random_device random_device;
	std::mt19937 random = std::mt19937(random_device());
};

inline Random g_random;
