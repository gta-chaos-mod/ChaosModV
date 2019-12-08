#pragma once
#include <random>
#include <time.h>

namespace Random
{
	void SetSeed(int seed);
	int GetRandomInt(int lower, int upper);

	namespace
	{
		std::default_random_engine m_random;
		int m_seed = -1;
	}
};