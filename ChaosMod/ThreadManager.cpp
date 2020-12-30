#include "stdafx.h"
#include "ThreadManager.h"

ThreadManager::ThreadManager() : m_mainThread(GetCurrentFiber())
{

}

DWORD64 ThreadManager::CreateThread(RegisteredEffect* effect)
{
	EffectThread thread(effect);

	m_threads.push_back(thread);

	return thread.GetId();
}

void ThreadManager::UnregisterThread(DWORD64 threadId)
{
	std::list<EffectThread>::iterator result = std::find(m_threads.begin(), m_threads.end(), threadId);

	if (result != m_threads.end())
	{
		m_threads.erase(result);
	}
}

void ThreadManager::PutThreadOnPause(DWORD time)
{
	void* fiber = GetCurrentFiber();

	std::list<EffectThread>::iterator result = std::find(m_threads.begin(), m_threads.end(), fiber);

	if (result != m_threads.end())
	{
		result->PauseTime = time;
	}
}

void ThreadManager::RunThreads()
{
	DWORD64 curTimestamp = GetTickCount64();

	for (EffectThread& thread : m_threads)
	{
		if (thread.PauseTime > 0)
		{
			thread.PauseTime -= curTimestamp - m_lastTimestamp;
		}

		if (thread.PauseTime == 0)
		{
			thread.Run();
		}
	}

	m_lastTimestamp = curTimestamp;
}

void ThreadManager::SwitchToMainThread()
{
	SwitchToFiber(m_mainThread);
}