/*
	Effect By Lucas7yoshi
*/

#include <stdafx.h>

static void OnTick()
{
	for (auto ped : GetAllPeds())
	{
		if (!IS_PED_RINGTONE_PLAYING(ped))
		{
			PLAY_PED_RINGTONE("Remote_Ring", ped, 1);
		}
	}
}

static void OnStop()
{
	for (auto ped : GetAllPeds())
	{
		STOP_PED_RINGTONE(ped);
	}
}

static RegisterEffect registerEffect(EFFECT_PHONES, nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Whose Phone Is Ringing?",
		.Id = "peds_phones",
		.IsTimed = true
	}
);