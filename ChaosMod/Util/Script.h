#pragma once

#include "../Effects/EffectThreads.h"

#include "../vendor/scripthookv/inc/main.h"

using DWORD                           = unsigned long;

inline void *g_MainThread             = nullptr;
inline void *g_EffectDispatcherThread = nullptr;

inline void WAIT(DWORD ulTimeMs)
{
	auto currentFiber = GetCurrentFiber();
	if (currentFiber == g_MainThread || currentFiber == g_EffectDispatcherThread)
	{
		scriptWait(ulTimeMs);
	}
	else
	{
		EffectThreads::PutThreadOnPause(ulTimeMs);

		EffectThreads::SwitchToEffectDispatcherThread();
	}
}