#include <stdafx.h>

#include "EffectThreads.h"

static std::list<std::unique_ptr<EffectThread>> m_rgThreads;
static DWORD64 m_ullLastTimestamp;

static EffectThread* ThreadIdToThread(DWORD64 ullThreadId)
{
	const auto& result = std::find(m_rgThreads.begin(), m_rgThreads.end(), ullThreadId);

	if (result != m_rgThreads.end())
	{
		return result->get();
	}

	return nullptr;
}

namespace EffectThreads
{
	DWORD64 CreateThread(RegisteredEffect* pEffect, bool bIsTimed)
	{
		std::unique_ptr<EffectThread> pThread = std::make_unique<EffectThread>(pEffect, bIsTimed);

		DWORD64 threadId = pThread->m_ullId;

		m_rgThreads.push_back(std::move(pThread));

		return threadId;
	}

	void StopThread(DWORD64 ullThreadId)
	{
		const auto& result = std::find(m_rgThreads.begin(), m_rgThreads.end(), ullThreadId);

		if (result != m_rgThreads.end())
		{
			result->get()->Stop();
		}
	}

	void StopThreads()
	{
		for (std::unique_ptr<EffectThread>& pThread : m_rgThreads)
		{
			pThread->Stop();
		}
	}

	void PutThreadOnPause(DWORD ulTimeMs)
	{
		PVOID fiber = GetCurrentFiber();

		const auto& ppResult = std::find(m_rgThreads.begin(), m_rgThreads.end(), fiber);

		if (ppResult != m_rgThreads.end())
		{
			(*ppResult)->m_iPauseTime = ulTimeMs;
		}
	}

	void RunThreads()
	{
		static int c_iLastFrame = GET_FRAME_COUNT();
		int iCurFrame = GET_FRAME_COUNT();

		if (c_iLastFrame == iCurFrame)
		{
			return;
		}

		c_iLastFrame = iCurFrame;

		DWORD64 ullCurTimestamp = GetTickCount64();

		for (auto it = m_rgThreads.begin(); it != m_rgThreads.end(); )
		{
			std::unique_ptr<EffectThread>& pThread = *it;

			if (pThread->HasStopped())
			{
				it = m_rgThreads.erase(it);

				continue;
			}

			if (pThread->m_iPauseTime > 0 && m_ullLastTimestamp)
			{
				pThread->m_iPauseTime -= ullCurTimestamp - m_ullLastTimestamp;
			}

			if (pThread->m_iPauseTime <= 0)
			{
				pThread->Run();
			}

			it++;
		}

		m_ullLastTimestamp = ullCurTimestamp;
	}

	void SwitchToMainThread()
	{
		SwitchToFiber(g_MainThread);
	}

	bool HasThreadOnStartExecuted(DWORD64 threadId)
	{
		EffectThread* pThread = ThreadIdToThread(threadId);

		return pThread ? pThread->HasOnStartExecuted() : false;
	}

	bool IsAnyThreadRunningOnStart()
	{
		for (const std::unique_ptr<EffectThread>& pThread : m_rgThreads)
		{
			if (!pThread->HasOnStartExecuted())
			{
				return true;
			}
		}

		return false;
	}

	bool IsAnyThreadRunning()
	{
		for (const std::unique_ptr<EffectThread>& pThread : m_rgThreads)
		{
			if (!pThread->HasStopped())
			{
				return true;
			}
		}

		return false;
	}

	void ClearThreads()
	{
		m_rgThreads.clear();
	}
}