#include <stdafx.h>

#include "Memory/Hooks/AudioPitchHook.h"

static int ms_fTargetPitch;

static void OnStart()
{
	ms_fTargetPitch = g_Random.GetRandomInt(750, 2000);
}

static void OnStop()
{
	Hooks::ResetAudioPitch();
}

static void OnTick()
{
	Hooks::SetAudioPitch(ms_fTargetPitch);
}

// clang-format off
REGISTER_EFFECT(OnStart, OnStop, OnTick, EffectInfo
	{
		.Name = "High Pitch",
		.Id = "misc_highpitch",
		.IsTimed = true,
		.EffectCategory = EEffectCategory::Pitch
	}
);