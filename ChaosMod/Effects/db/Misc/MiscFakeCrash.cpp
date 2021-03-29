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
	if (g_random.GetRandomInt(0, 1))
	{
		g_effectDispatcher->FakeTimerBarPercentage = g_random.GetRandomFloat(0.f, 1.f);
	}

	SleepAllThreads(500);

	WAIT(500);

	SleepAllThreads(g_random.GetRandomInt(3000, 5000));

	g_effectDispatcher->FakeTimerBarPercentage = 0.f;
}

static RegisterEffect registerEffect(EFFECT_MISC_CRASH, OnStart, EffectInfo
	{
		.Name = "Fake Crash",
		.Id = "misc_fakecrash"
	}
);