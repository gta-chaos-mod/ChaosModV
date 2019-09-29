#pragma once
#include <random>

namespace Random
{
	static int GetRandomInt(int lower, int upper)
	{
		static std::random_device random;
		static std::mt19937 eng(random()); // TODO: allow for user seed specification
		std::uniform_int_distribution<> distr(lower, upper);
		return distr(eng);
	}
}