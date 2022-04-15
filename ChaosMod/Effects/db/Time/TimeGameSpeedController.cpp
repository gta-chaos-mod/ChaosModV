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

static RegisterEffect registerEffect1(EFFECT_GAMESPEED_X02, nullptr, OnStop, OnTickX02, EffectInfo
	{
		.Name = "x0.2 Gamespeed",
		.Id = "time_x02",
		.IsTimed = true,
		.IsShortDuration = true,
		.IncompatibleWith = { EFFECT_HIGH_PITCH, EFFECT_LOW_PITCH, EFFECT_WEIRD_PITCH, EFFECT_PEDS_MINIONS, EFFECT_GAMESPEED_X05, EFFECT_PLAYER_DEAD_EYE, EFFECT_GAMESPEED_SUPERHOT }
	}
);
static void OnTickX05()
{
	SET_AUDIO_FLAG("AllowScriptedSpeechInSlowMo", true);
	SET_AUDIO_FLAG("AllowAmbientSpeechInSlowMo", true);

	Hooks::SetAudioPitch(-500);

	SET_TIME_SCALE(.5f);
}

static RegisterEffect registerEffect2(EFFECT_GAMESPEED_X05, nullptr, OnStop, OnTickX05, EffectInfo
	{
		.Name = "x0.5 Gamespeed",
		.Id = "time_x05",
		.IsTimed = true,
		.IsShortDuration = true,
		.IncompatibleWith = { EFFECT_HIGH_PITCH, EFFECT_LOW_PITCH, EFFECT_WEIRD_PITCH, EFFECT_PEDS_MINIONS, EFFECT_GAMESPEED_X02, EFFECT_PLAYER_DEAD_EYE, EFFECT_GAMESPEED_SUPERHOT }
	}
);