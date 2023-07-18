/*
    Effect by Last0xygen, modified
*/

#include <stdafx.h>

#include "Memory/Hooks/AudioPitchHook.h"

static void OnStop()
{
	Hooks::ResetAudioPitch();

	for (Ped pd : GetAllPeds())
	{
		if (GET_PED_CONFIG_FLAG(pd, 223, true))
		{
			SET_PED_CONFIG_FLAG(pd, 223, false);
		}
	}
}

static void OnTick()
{
	Hooks::SetAudioPitch(300);

	for (Ped pd : GetAllPeds())
	{
		if (!GET_PED_CONFIG_FLAG(pd, 223, true))
		{
			SET_PED_CONFIG_FLAG(pd, 223, true);
		}
	}
}

// clang-format off
REGISTER_EFFECT(nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Minions",
		.Id = "peds_minions",
		.IsTimed = true,
		.EffectCategory = EffectCategory::Pitch
	}
);