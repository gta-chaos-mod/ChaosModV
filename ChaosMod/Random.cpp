#include "stdafx.h"

#include "Random.h"

int m_seed = -1;

namespace Random
{
	void SetSeed(int seed)
	{
		m_seed = seed;
	}

	int GetRandomInt(int lower, int upper)
	{
		static std::default_random_engine random1;
		static std::random_device random_device;
		static std::mt19937 random2(random_device());

		static bool firstTime = true;
		static bool useNewRandom = false;
		if (firstTime)
		{
			firstTime = false;

			if (m_seed >= 0)
			{
				random1.seed(m_seed);
			}
			else
			{
				useNewRandom = true;
			}
		}

		std::uniform_int_distribution<> distr(lower, upper);
		return distr(useNewRandom ? random2 : random1);
	}
}