#include "stdafx.h"
#include "Random.h"

std::default_random_engine m_random;
int m_seed = -1;

namespace Random
{
	void SetSeed(int seed)
	{
		m_seed = seed;
	}

	int GetRandomInt(int lower, int upper)
	{
		static bool firstTime = true;
		if (firstTime)
		{
			firstTime = false;

			if (m_seed >= 0)
			{
				m_random.seed(m_seed);
			}
			else
			{
				srand(time(nullptr));
				m_random.seed(rand());
			}
		}

		std::uniform_int_distribution<> distr(lower, upper);
		return distr(m_random);
	}
}