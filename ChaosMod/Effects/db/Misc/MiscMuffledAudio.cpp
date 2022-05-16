#include "stdafx.h"
#include "Memory/Hooks/AudioClearnessHook.h"

static void OnStart()
{
	Hooks::SetAudioClearness(0);
}

static void OnStop()
{
	Hooks::ResetAudioClearness();
}

static RegisterEffect registerEffect(EFFECT_MISC_AUDIO_MUFFLED, OnStart, OnStop, EffectInfo
	{
		.Name = "Muffled Audio",
		.Id = "misc_audio_muffled",
		.IsTimed = true,
		.IncompatibleWith = { EFFECT_HIGH_PITCH, EFFECT_LOW_PITCH, EFFECT_WEIRD_PITCH, EFFECT_PEDS_MINIONS, EFFECT_GAMESPEED_X02, EFFECT_GAMESPEED_X05 }
	}
);