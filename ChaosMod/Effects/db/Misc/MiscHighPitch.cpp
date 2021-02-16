#include <stdafx.h>

#include "Memory/Hooks/AudioPitchHook.h"

static int m_targetPitch;

static void OnStart()
{
	m_targetPitch = g_random.GetRandomInt(750, 2000);
}

static void OnStop()
{
	Hooks::ResetAudioPitch();
}

static void OnTick()
{
	Hooks::SetAudioPitch(m_targetPitch);
}

static RegisterEffect registerEffect(EFFECT_HIGH_PITCH, OnStart, OnStop, OnTick);