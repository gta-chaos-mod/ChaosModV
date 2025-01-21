#include <stdafx.h>

#include "Effects/Register/RegisterEffect.h"
#include "Memory/Hooks/AudioPitchHook.h"

CHAOS_VAR int ms_TargetPitch;

static void OnStart()
{
	ms_TargetPitch = g_Random.GetRandomInt(750, 2000);
}

static void OnStop()
{
	Hooks::ResetAudioPitch();
}

static void OnTick()
{
	Hooks::SetAudioPitch(ms_TargetPitch);
}

// clang-format off
REGISTER_EFFECT(OnStart, OnStop, OnTick, 
	{
		.Name = "High Pitch",
		.Id = "misc_highpitch",
		.IsTimed = true,
		.EffectCategory = EffectCategory::Pitch
	}
);