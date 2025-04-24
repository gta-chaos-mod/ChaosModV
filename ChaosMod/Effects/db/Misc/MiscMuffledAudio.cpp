/*
    Effect By Rylxnd
*/
#include <stdafx.h>

#include "Effects/Register/RegisterEffect.h"
#include "Memory/Hooks/AudioSettingsHook.h"

static void OnStart()
{
	Hooks::SetAudioLPFCutoff(100);
}

static void OnStop()
{
	Hooks::ResetAudioLPFCutoff();
}

// clang-format off
REGISTER_EFFECT(OnStart, OnStop, nullptr, 
	{
		.Name = "Muffled Audio",
		.Id = "misc_muffled_audio",
		.IsTimed = true,
	}
);
