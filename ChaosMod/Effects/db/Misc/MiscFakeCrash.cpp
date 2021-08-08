#include <stdafx.h>

static void SleepAllThreads(DWORD ms)
{
	std::vector<HANDLE> threads;

	HANDLE handle = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	
	THREADENTRY32 threadEntry {};
	threadEntry.dwSize = sizeof(threadEntry);

	Thread32First(handle, &threadEntry);
	do
	{
		if (threadEntry.dwSize >= FIELD_OFFSET(THREADENTRY32, th32OwnerProcessID) + sizeof(threadEntry.th32OwnerProcessID))
		{
			if (threadEntry.th32ThreadID != GetCurrentThreadId() && threadEntry.th32OwnerProcessID == GetCurrentProcessId())
			{
				HANDLE thread = OpenThread(THREAD_ALL_ACCESS, FALSE, threadEntry.th32ThreadID);

				threads.push_back(thread);
			}
		}

		threadEntry.dwSize = sizeof(threadEntry);
	}
	while (Thread32Next(handle, &threadEntry));

	for (HANDLE thread : threads)
	{
		SuspendThread(thread);
	}

	Sleep(ms);

	for (HANDLE thread : threads)
	{
		ResumeThread(thread);
		CloseHandle(thread);
	}

	CloseHandle(handle);
}

static void OnStart()
{
	bool fakeTimer = g_Random.GetRandomInt(0, 1);

	if (fakeTimer)
	{
		g_pEffectDispatcher->m_fFakeTimerBarPercentage = g_Random.GetRandomFloat(0.f, 1.f);
	}

	// sometimes it may just freeze, other times it could jump ahead a bit
	if (g_Random.GetRandomInt(0, 1))
	{
		SleepAllThreads(500);
		WAIT(500);
	}
	
	// another devilish option, sometimes the chaos mod ui may just up and vanish
	if (g_Random.GetRandomInt(0, 1))
	{
		g_MetaInfo.m_bShouldHideChaosUI = true;
	}

	SleepAllThreads(g_Random.GetRandomInt(3000, 6000));

	if (fakeTimer)
	{
		g_pEffectDispatcher->m_fFakeTimerBarPercentage = 0.f;
	}
	
	g_MetaInfo.m_bShouldHideChaosUI = false;
}

static RegisterEffect registerEffect(EFFECT_MISC_CRASH, OnStart, EffectInfo
	{
		.Name = "Fake Crash",
		.Id = "misc_fakecrash"
	}
);
