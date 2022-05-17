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

static RegisterEffect registerEffect(EFFECT_MISC_DEAF, OnStart, OnStopAll, EffectInfo
	{
		.Name = "Deaf",
		.Id = "misc_deaf",
		.IsTimed = true,
		.IncompatibleWith = {EFFECT_MISC_SUPER_HEARING},
		.EffectGroupType = EEffectGroupType::Audio
	}
);