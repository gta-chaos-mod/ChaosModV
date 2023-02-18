#include <stdafx.h>

#include "Memory/Hooks/AudioPitchHook.h"

static void OnStop()
{
	SET_AUDIO_FLAG("AllowScriptedSpeechInSlowMo", false);
	SET_AUDIO_FLAG("AllowAmbientSpeechInSlowMo", false);

	Hooks::ResetAudioPitch();

	SET_TIME_SCALE(1.f);
}

static void OnTickX02()
{
	SET_AUDIO_FLAG("AllowScriptedSpeechInSlowMo", true);
	SET_AUDIO_FLAG("AllowAmbientSpeechInSlowMo", true);

	Hooks::SetAudioPitch(-900);

	SET_TIME_SCALE(.2f);
}

// clang-format off
REGISTER_EFFECT(nullptr, OnStop, OnTickX02, EffectInfo
	{
		.Name = "x0.2 Gamespeed",
		.Id = "time_x02",
		.IsTimed = true,
		.IsShortDuration = true,
		.IncompatibleWith = { "time_x05", "player_dead_eye", "time_superhot" },
		.EffectCategory = EEffectCategory::Pitch
	}
);
// clang-format on

static void OnTickX05()
{
	SET_AUDIO_FLAG("AllowScriptedSpeechInSlowMo", true);
	SET_AUDIO_FLAG("AllowAmbientSpeechInSlowMo", true);

	Hooks::SetAudioPitch(-500);

	SET_TIME_SCALE(.5f);
}

// clang-format off
REGISTER_EFFECT(nullptr, OnStop, OnTickX05, EffectInfo
	{
		.Name = "x0.5 Gamespeed",
		.Id = "time_x05",
		.IsTimed = true,
		.IsShortDuration = true,
		.IncompatibleWith = { "time_x02", "player_dead_eye", "time_superhot" },
		.EffectCategory = EEffectCategory::Pitch
	}
);