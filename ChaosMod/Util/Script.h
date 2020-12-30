#pragma once

#include "ThreadManager.h"

typedef unsigned long DWORD;

inline void WAIT(DWORD ms)
{
	g_threadManager->PutThreadOnPause(ms);

	g_threadManager->SwitchToMainThread();
}