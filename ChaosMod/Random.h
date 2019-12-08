#pragma once
#include <random>
#include <time.h>

namespace Random
{
	void SetSeed(int seed);
	int GetRandomInt(int lower, int upper);
};