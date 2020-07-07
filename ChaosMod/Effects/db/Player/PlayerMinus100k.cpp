#include <stdafx.h>

static void OnStart()
{
	for (int i = 0; i < 3; i++)
	{
		char statNameFull[32];
		sprintf_s(statNameFull, "SP%d_TOTAL_CASH", i);

		Hash hash = GET_HASH_KEY(statNameFull);

		STAT_SET_INT(hash, 0, true);
	}
}

static RegisterEffect registerEffect(EFFECT_MINUS_100K, OnStart);