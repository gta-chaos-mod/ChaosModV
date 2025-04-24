#include <stdafx.h>

#include "Effects/Register/RegisterEffect.h"
#include "Memory/Hooks/AudioSettingsHook.h"

CHAOS_VAR float ms_TargetPitch;

static void OnStart()
{
	ms_TargetPitch = g_Random.GetRandomFloat(750.f, 2000.f);
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