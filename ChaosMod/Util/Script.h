#pragma once

#include "../EffectThreads.h"

#include "../vendor/scripthookv/inc/main.h"

using DWORD = unsigned long;

inline void* g_MainThread = nullptr;

inline void WAIT(DWORD ulTimeMs)
{
	if (!g_MainThread || GetCurrentFiber() == g_MainThread)
	{
		scriptWait(ulTimeMs);
	}
	else
	{
		EffectThreads::PutThreadOnPause(ulTimeMs);

		EffectThreads::SwitchToMainThread();
	}
}