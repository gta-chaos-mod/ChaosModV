#include <stdafx.h>

#include "Memory/Hooks/AudioPitchHook.h"

//Effect by ProfessorBiddle, but the code is pretty much copied and pasted

static int ms_fTargetPitch;

static void OnStart()
{
	ms_fTargetPitch = g_Random.GetRandomInt(-900, -300);
}

static void OnStop()
{
	Hooks::ResetAudioPitch();
}

static void OnTick()
{
	Hooks::SetAudioPitch(ms_fTargetPitch);
}

static RegisterEffect registerEffect(EFFECT_LOW_PITCH, OnStart, OnStop, OnTick, EffectInfo
	{
		// These are always required, you may have to add more designators depending on your effect
		.Name = "Low Pitch",
		.Id = "misc_lowpitch",
		.IsTimed = true,
		.IncompatibleWith = { EFFECT_HIGH_PITCH }
	}
);