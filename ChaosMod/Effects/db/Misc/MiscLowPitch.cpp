#include <stdafx.h>

#include "Memory/Hooks/AudioPitchHook.h"

//Effect by ProfessorBiddle, but the code is pretty much copied and pasted

static int m_targetPitch;

static void OnStart()
{
	m_targetPitch = g_random.GetRandomInt(-900, -300);

	SET_AUDIO_FLAG("AllowScriptedSpeechInSlowMo", true);
	SET_AUDIO_FLAG("AllowAmbientSpeechInSlowMo", true);
}

static void OnStop()
{
	Hooks::ResetAudioPitch();

	SET_AUDIO_FLAG("AllowScriptedSpeechInSlowMo", false);
	SET_AUDIO_FLAG("AllowAmbientSpeechInSlowMo", false);
}

static void OnTick()
{
	Hooks::SetAudioPitch(m_targetPitch);
}

static RegisterEffect registerEffect(EFFECT_LOW_PITCH, OnStart, OnStop, OnTick);