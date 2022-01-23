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

static RegisterEffect registerEffect(EFFECT_HIGH_PITCH, OnStart, OnStop, OnTick, EffectInfo
	{
		.Name = "High Pitch",
		.Id = "misc_highpitch",
		.IsTimed = true,
		.IncompatibleWith = { EFFECT_LOW_PITCH, EFFECT_WEIRD_PITCH, EFFECT_PEDS_MINIONS, EFFECT_GAMESPEED_X02, EFFECT_GAMESPEED_X05 }
	}
);