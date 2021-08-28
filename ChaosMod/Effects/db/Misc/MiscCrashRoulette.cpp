#include <stdafx.h>

#define LAUNCH_TIMER 6000

static void SleepAllThreads(DWORD ms)
{
	std::vector<HANDLE> threads;

	HANDLE handle = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);

	THREADENTRY32 threadEntry{};
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
	} while (Thread32Next(handle, &threadEntry));

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
	int lastTimestamp = GET_GAME_TIMER();
	int rouletteTimer = LAUNCH_TIMER;
	int crashTimer = LAUNCH_TIMER;
	while (true)
	{

		int curTimestamp = GET_GAME_TIMER();

		if ((rouletteTimer -= curTimestamp - lastTimestamp) < crashTimer)
		{
			crashTimer *= .8f;
			SleepAllThreads(100);

		}

		if (rouletteTimer <= 0)
		{
			int crashPercentage = (g_Random.GetRandomInt(1, 10));
			if (crashPercentage <= 2)
			{
				*(volatile int*)0 = 0;
			}
			break;
		}

		lastTimestamp = curTimestamp;

		WAIT(0);
	}
}

static RegisterEffect registerEffect(EFFECT_MISC_CRASHROULETTE, OnStart, EffectInfo
	{
		.Name = "Crash Roulette",
		.Id = "misc_crashroulette"
	}
);