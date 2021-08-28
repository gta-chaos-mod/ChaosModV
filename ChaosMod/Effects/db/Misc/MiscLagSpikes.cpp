#include <stdafx.h>

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

static void OnTickSpike()
{
	SleepAllThreads(g_Random.GetRandomInt(40, 400));
	WAIT(g_Random.GetRandomInt(600, 1300));
}

static RegisterEffect registerEffect3(EFFECT_MISC_LAGSPIKES, nullptr, nullptr, OnTickSpike, EffectInfo
	{
		.Name = "Lagspikes",
		.Id = "misc_lagspikes",
		.IsTimed = true,
		.IsShortDuration = false
	}
);