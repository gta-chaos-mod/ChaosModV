#include <stdafx.h>

#include "Memory/Hooks/AudioClearnessHook.h"

static void OnStart()
{
	Hooks::SetAudioClearness(0);
}

static void OnStop()
{
	Hooks::ResetAudioClearness();
}

REGISTER_EFFECT(OnStart, OnStop, nullptr, EffectInfo
	{
		.Name = "Muffled Audio",
		.Id = "misc_audio_muffled",
		.IsTimed = true,
	}
);
