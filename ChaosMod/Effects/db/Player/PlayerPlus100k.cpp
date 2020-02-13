#include <stdafx.h>

static void OnStart()
{
	for (int i = 0; i < 3; i++)
	{
		char statNameFull[32];
		sprintf_s(statNameFull, "SP%d_TOTAL_CASH", i);

		auto hash = GET_HASH_KEY(statNameFull);

		int money;
		STAT_GET_INT(hash, &money, -1);
		STAT_SET_INT(hash, money + 100000, 1);
	}
}

static RegisterEffect registerEffect(EFFECT_PLUS_100K, OnStart);