#include <stdafx.h>

/*
	Effect by Last0xygen
*/

static void OnTick()
{
	int time = 1000 / 15;
	int lastUpdateTick = GetTickCount64();
	while (lastUpdateTick > GetTickCount64() - time)
	{
		// Create Lag
	}
}

static RegisterEffect registerEffect(EFFECT_MISC_FPS_LIMIT, nullptr, nullptr, OnTick);
