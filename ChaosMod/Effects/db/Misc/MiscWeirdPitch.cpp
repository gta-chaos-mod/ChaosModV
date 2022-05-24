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

// clang-format off
REGISTER_EFFECT(nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Weird Pitch",
		.Id = "misc_weirdpitch",
		.IsTimed = true,
		.EffectCategory = EEffectCategory::Pitch
	}
);