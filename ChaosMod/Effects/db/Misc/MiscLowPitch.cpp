#include <stdafx.h>

#include "Effects/Register/RegisterEffect.h"
#include "Memory/Hooks/AudioPitchHook.h"

// Effect by ProfessorBiddle, but the code is pretty much copied and pasted

CHAOS_VAR int ms_TargetPitch;

static void OnStart()
{
	ms_TargetPitch = g_Random.GetRandomInt(-900, -300);
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
		.Name = "Low Pitch",
		.Id = "misc_lowpitch",
		.IsTimed = true,
		.EffectCategory = EffectCategory::Pitch
	}
);