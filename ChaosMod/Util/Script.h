#pragma once

#include "ThreadManager.h"

#include "../vendor/scripthookv/inc/main.h"

typedef unsigned long DWORD;

inline void* g_mainThread = nullptr;

inline void WAIT(DWORD ms)
{
	if (!g_mainThread || GetCurrentFiber() == g_mainThread)
	{
		scriptWait(ms);
	}
	else
	{
		ThreadManager::PutThreadOnPause(ms);

		ThreadManager::SwitchToMainThread();
	}
}