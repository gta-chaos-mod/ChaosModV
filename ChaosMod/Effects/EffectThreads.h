#pragma once

#include "Effects/Effect.h"

#include "Util/CrashHandler.h"

#include <memory>

using DWORD   = unsigned long;
using DWORD64 = unsigned long long;

using LPVOID  = void *;

namespace EffectThreads
{
	LPVOID CreateThread(RegisteredEffect *pEffect, bool bIsTimed);

	void StopThread(LPVOID threadId);
	void StopThreadImmediately(LPVOID threadId);
	void StopThreads();
	void StopThreadsImmediately();

	void PauseThisThread(DWORD ulTimeMs);
	bool IsThreadPaused(LPVOID threadId);

	void RunThreads();
	void RunThread(LPVOID threadId);

	bool DoesThreadExist(LPVOID threadId);
	bool HasThreadOnStartExecuted(LPVOID threadId);
};

struct EffectThreadData
{
	RegisteredEffect *m_Effect = nullptr;
	bool *m_HasOnStartExecuted = nullptr;
	bool *m_IsRunning          = nullptr;
	bool *m_HasStopped         = nullptr;

	void *m_CallerFiber        = nullptr;

	EffectThreadData(RegisteredEffect *pEffect, bool *bHasOnStartExecuted, bool *bIsRunning, bool *bHasStopped)
	    : m_Effect(pEffect),
	      m_HasOnStartExecuted(bHasOnStartExecuted),
	      m_IsRunning(bIsRunning),
	      m_HasStopped(bHasStopped)
	{
	}
};

inline void EffectThreadFunc(LPVOID pData)
{
	SetUnhandledExceptionFilter(CrashHandler);

	EffectThreadData threadData = *reinterpret_cast<EffectThreadData *>(pData);

	threadData.m_Effect->Start();
	*threadData.m_HasOnStartExecuted = true;

	while (*threadData.m_IsRunning)
	{
		SwitchToFiber(threadData.m_CallerFiber);
		threadData.m_Effect->Tick();
	}

	threadData.m_Effect->Stop();

	*threadData.m_HasStopped = true;
	SwitchToFiber(threadData.m_CallerFiber);
}

class EffectThread
{
  private:
	RegisteredEffect *m_Effect = nullptr;
	bool m_HasOnStartExecuted  = false;
	bool m_IsRunning           = false;
	bool m_HasStopped          = false;
	EffectThreadData m_ThreadData;

  public:
	DWORD64 m_PauseTimestamp = 0;
	LPVOID m_Thread          = nullptr;

	EffectThread(RegisteredEffect *pEffect, bool bIsTimed)
	    : m_Effect(pEffect),
	      m_IsRunning(bIsTimed),
	      m_ThreadData(pEffect, &m_HasOnStartExecuted, &m_IsRunning, &m_HasStopped),
	      m_Thread(CreateFiber(0, EffectThreadFunc, &m_ThreadData))
	{
	}

	~EffectThread()
	{
		DeleteFiber(m_Thread);
	}

	EffectThread(const EffectThread &)            = delete;

	EffectThread &operator=(const EffectThread &) = delete;

	friend bool operator==(const std::unique_ptr<EffectThread> &pThisThread, LPVOID pThread)
	{
		return pThisThread->m_Thread == pThread;
	}

	inline void OnRun()
	{
		m_ThreadData.m_CallerFiber = GetCurrentFiber();
		SwitchToFiber(m_Thread);
	}

	inline void Stop()
	{
		if (!m_HasStopped)
		{
			m_IsRunning = false;

			OnRun();
		}
	}

	inline bool HasStopped() const
	{
		return m_HasStopped;
	}

	inline bool HasOnStartExecuted() const
	{
		return m_HasOnStartExecuted;
	}

	inline bool IsStopping() const
	{
		return !m_IsRunning;
	}
};
