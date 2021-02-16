#include "stdafx.h"
#include "ThreadManager.h"

static std::list<std::unique_ptr<EffectThread>> m_threads;
static DWORD64 m_lastTimestamp;

static EffectThread* ThreadIdToThread(DWORD64 threadId)
{
	const std::list<std::unique_ptr<EffectThread>>::iterator& result = std::find(m_threads.begin(), m_threads.end(), threadId);

	if (result != m_threads.end())
	{
		return result->get();
	}

	return nullptr;
}

namespace ThreadManager
{
	DWORD64 CreateThread(RegisteredEffect* effect, bool isTimed)
	{
		std::unique_ptr<EffectThread> thread = std::make_unique<EffectThread>(effect, isTimed);

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
		}
	}

	void StopThreads()
	{
		for (std::unique_ptr<EffectThread>& thread : m_threads)
		{
			thread->Stop();
		}
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
		static int lastFrame = GET_FRAME_COUNT();
		int curFrame = GET_FRAME_COUNT();

		if (lastFrame == curFrame)
		{
			return;
		}

		lastFrame = curFrame;

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

	bool HasThreadOnStartExecuted(DWORD64 threadId)
	{
		EffectThread* thread = ThreadIdToThread(threadId);

		return thread ? thread->HasOnStartExecuted() : false;
	}

	bool IsAnyThreadRunningOnStart()
	{
		for (const std::unique_ptr<EffectThread>& thread : m_threads)
		{
			if (!thread->HasOnStartExecuted())
			{
				return true;
			}
		}

		return false;
	}

	bool IsAnyThreadRunning()
	{
		for (const std::unique_ptr<EffectThread>& thread : m_threads)
		{
			if (!thread->HasStopped())
			{
				return true;
			}
		}

		return false;
	}

	void ClearThreads()
	{
		m_threads.clear();
	}
}