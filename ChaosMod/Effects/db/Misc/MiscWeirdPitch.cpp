#include <stdafx.h>

#include "Memory/Hooks/AudioPitchHook.h"

static void OnStop()
{
	Hooks::ResetAudioPitch();
}

static void OnTick()
{
	Hooks::SetAudioPitch(g_Random.GetRandomInt(-900, 2000));
}

static RegisterEffect registerEffect(nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Weird Pitch",
		.Id = "misc_weirdpitch",
		.IsTimed = true,
		.EffectGroupType = EEffectGroupType::Audio
	}
);