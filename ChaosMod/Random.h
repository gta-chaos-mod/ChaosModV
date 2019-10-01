#pragma once
#include <random>
#include <time.h>

class Random
{
private:
	Random() {}

public:
	static inline void SetSeed(int seed)
	{
		m_seed = seed;
	}

	static int GetRandomInt(int lower, int upper)
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

private:
	static inline std::default_random_engine m_random;
	static inline int m_seed = -1;
};