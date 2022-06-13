/*
	Effect By OnlyRealNubs
*/

#include "stdafx.h"

#include "Memory/Hooks/AudioVolumeHook.h"

static void OnStart()
{
	Hooks::SetAudioVolume(-100);
}

static void OnStop()
{
	Hooks::ResetAudioVolume();
}

// clang-format off
REGISTER_EFFECT(OnStart, OnStop, nullptr, EffectInfo
	{
		.Name = "Deaf",
		.Id = "misc_deaf",
		.IsTimed = true,
		.EffectGroupType = EEffectGroupType::Audio
	}
);
