#pragma once

#include "Effects/EffectThreads.h"

#include <scripthookv/inc/main.h>

using DWORD                           = unsigned long;

inline void *g_MainThread             = nullptr;
inline void *g_EffectDispatcherThread = nullptr;

inline void WAIT(DWORD timeMs)
{
	auto currentFiber = GetCurrentFiber();
	if (currentFiber == g_MainThread || currentFiber == g_EffectDispatcherThread)
	{
		scriptWait(timeMs);
	}
	else
	{
		EffectThreads::PauseThisThread(timeMs);

		if (g_EffectDispatcherThread)
		{
			SwitchToFiber(g_EffectDispatcherThread);
		}
		else
		{
			SwitchToFiber(g_MainThread);
		}
	}
}