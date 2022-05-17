/*
	Effect By OnlyRealNubs
*/

#include "stdafx.h"

#include "Memory/Hooks/AudioVolumeHook.h"

static void OnStart()
{
	Hooks::SetAudioVolume(0.029678);
}

static void OnStop()
{
	Hooks::ResetAudioVolume();
}

static RegisterEffect registerEffect2(EFFECT_MISC_SUPER_HEARING, OnStart, OnStop, EffectInfo
	{
		.Name = "Super Hearing",
		.Id = "misc_super_hearing",
		.IsTimed = true,
		.IncompatibleWith = {EFFECT_MISC_DEAF},
		.EffectGroupType = EEffectGroupType::Audio
	}
);