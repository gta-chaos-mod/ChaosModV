#include <stdafx.h>

#include "Memory/Hooks/AudioPitchHook.h"

// Effect by ProfessorBiddle, but the code is pretty much copied and pasted

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

// clang-format off
REGISTER_EFFECT(OnStart, OnStop, OnTick, EffectInfo
	{
		.Name = "Low Pitch",
		.Id = "misc_lowpitch",
		.IsTimed = true,
		.EffectCategory = EEffectCategory::Pitch
	}
);