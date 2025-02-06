#include <stdafx.h>

#include "Effects/Register/RegisterEffect.h"
#include "Memory/Hooks/AudioPitchHook.h"

static void OnStop()
{
	Hooks::ResetAudioPitch();
}

static void OnTick()
{
	Hooks::SetAudioPitch(g_Random.GetRandomFloat(-900.f, 2000.f));
}

// clang-format off
REGISTER_EFFECT(nullptr, OnStop, OnTick, 
	{
		.Name = "Weird Pitch",
		.Id = "misc_weirdpitch",
		.IsTimed = true,
		.EffectCategory = EffectCategory::Pitch
	}
);