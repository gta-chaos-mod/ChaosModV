#include <stdafx.h>

static void OnTick()
{
	Memory::SetSkyDisabled(true);
}

static void OnStop()
{
	Memory::SetSkyDisabled(false);
}

static RegisterEffect registerEffect(EFFECT_NO_SKY, nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "No Sky",
		.Id = "misc_nosky",
		.IsTimed = true
	}
);