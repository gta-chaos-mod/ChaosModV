#include <stdafx.h>

/*
	Effect by Last0xygen
*/

static void OnTick()
{
	static const int lagTimeDelay = 1000 / 25;
	int lastUpdateTick = GetTickCount64();
	while (lastUpdateTick > GetTickCount64() - lagTimeDelay)
	{
		// Create Lag
	}
}

static RegisterEffect registerEffect(EFFECT_MISC_FPS_LIMIT, nullptr, nullptr, OnTick);
