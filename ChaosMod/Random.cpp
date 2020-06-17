#include "stdafx.h"

#include "Random.h"

namespace Random
{
	int m_seed = 0;

	void SetSeed(int seed)
	{
		m_seed = seed;
	}

	int GetRandomInt(int lower, int upper)
	{
		static std::random_device random_device;
		static std::mt19937 random(random_device());

		static bool firstTime = true;
		if (firstTime)
		{
			firstTime = false;

			if (m_seed > 0)
			{
				random.seed(m_seed);
			}
		}

		std::uniform_int_distribution<> distr(lower, upper);
		return distr(random);
	}
}