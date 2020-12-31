#include "stdafx.h"
#include "ThreadManager.h"

static std::list<std::unique_ptr<EffectThread>> m_threads;
static DWORD64 m_lastTimestamp;

namespace ThreadManager
{
	DWORD64 CreateThread(RegisteredEffect* effect, bool isTimed, std::string* name)
	{
		std::unique_ptr<EffectThread> thread = std::make_unique<EffectThread>(effect, isTimed, name);

		DWORD64 threadId = thread->Id;

		m_threads.push_back(std::move(thread));

		return threadId;
	}

	void StopThread(DWORD64 threadId)
	{
		const std::list<std::unique_ptr<EffectThread>>::iterator& result = std::find(m_threads.begin(), m_threads.end(), threadId);

		if (result != m_threads.end())
		{
			result->get()->Stop();

			m_threads.erase(result);
		}
	}

	void StopThreads()
	{
		for (std::unique_ptr<EffectThread>& thread : m_threads)
		{
			thread->Stop();
		}

		m_threads.clear();
	}

	void PutThreadOnPause(DWORD ms)
	{
		void* fiber = GetCurrentFiber();

		const std::list<std::unique_ptr<EffectThread>>::iterator& result = std::find(m_threads.begin(), m_threads.end(), fiber);

		if (result != m_threads.end())
		{
			result->get()->PauseTime = ms;
		}
	}

	void RunThreads()
	{
		DWORD64 curTimestamp = GetTickCount64();

		for (std::list<std::unique_ptr<EffectThread>>::iterator it = m_threads.begin(); it != m_threads.end(); )
		{
			std::unique_ptr<EffectThread>& thread = *it;

			if (thread->HasStopped())
			{
				it = m_threads.erase(it);

				continue;
			}

			if (thread->PauseTime > 0 && m_lastTimestamp)
			{
				thread->PauseTime -= curTimestamp - m_lastTimestamp;
			}

			if (thread->PauseTime <= 0)
			{
				thread->Run();
			}

			it++;
		}

		m_lastTimestamp = curTimestamp;
	}

	void SwitchToMainThread()
	{
		SwitchToFiber(g_mainThread);
	}
}