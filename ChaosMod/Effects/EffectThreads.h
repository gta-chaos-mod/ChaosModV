#pragma once

#include "Effects/Effect.h"

#include "Util/CrashHandler.h"

#include <memory>

using DWORD   = unsigned long;
using DWORD64 = unsigned long long;

using LPVOID  = void *;

namespace EffectThreads
{
	DWORD64 CreateThread(RegisteredEffect *pEffect, bool bIsTimed);

	void StopThread(DWORD64 threadId);
	void StopThreadImmediately(DWORD64 threadId);
	void StopThreads();
	void StopThreadsImmediately();

	void PutThreadOnPause(DWORD ulTimeMs);

	void RunThreads();

	bool DoesThreadExist(DWORD64 threadId);
	bool HasThreadOnStartExecuted(DWORD64 threadId);
	bool HasThreadStopped(DWORD64 threadId);

	bool IsAnyThreadRunningOnStart();
	bool IsAnyThreadRunning();

	void ClearThreads();
};

struct EffectThreadData
{
	RegisteredEffect *m_pEffect;
	bool &m_bHasOnStartExecuted;
	bool &m_bIsRunning;
	bool &m_bHasStopped;

	void *m_CallerFiber = nullptr;

	EffectThreadData(RegisteredEffect *pEffect, bool &bHasOnStartExecuted, bool &bIsRunning, bool &bHasStopped)
	    : m_pEffect(pEffect),
	      m_bHasOnStartExecuted(bHasOnStartExecuted),
	      m_bIsRunning(bIsRunning),
	      m_bHasStopped(bHasStopped)
	{
	}
};

inline void EffectThreadFunc(LPVOID pData)
{
	SetUnhandledExceptionFilter(CrashHandler);

	extern void WAIT(DWORD ulTimeMs, void *callerFiber);

	EffectThreadData threadData = *reinterpret_cast<EffectThreadData *>(pData);

	threadData.m_pEffect->Start();

	threadData.m_bHasOnStartExecuted = true;

	while (threadData.m_bIsRunning)
	{
		threadData.m_pEffect->Tick();

		SwitchToFiber(threadData.m_CallerFiber);
	}

	threadData.m_pEffect->Stop();

	threadData.m_bHasStopped = true;

	SwitchToFiber(threadData.m_CallerFiber);
}

class EffectThread
{
  private:
	static inline DWORD64 ms_ullLastId = 0;

	RegisteredEffect *m_pEffect;
	bool m_bHasOnStartExecuted = false;
	bool m_bIsRunning;
	bool m_bHasStopped = false;
	EffectThreadData m_ThreadData;

  public:
	int m_iPauseTime = 0;
	LPVOID m_pThread;
	const DWORD64 m_ullId = 0;

	EffectThread(RegisteredEffect *pEffect, bool bIsTimed)
	    : m_pEffect(pEffect),
	      m_bIsRunning(bIsTimed),
	      m_ThreadData(pEffect, m_bHasOnStartExecuted, m_bIsRunning, m_bHasStopped),
	      m_pThread(CreateFiber(0, EffectThreadFunc, &m_ThreadData)),
	      m_ullId(ms_ullLastId++)
	{
	}

	~EffectThread()
	{
		DeleteFiber(m_pThread);
	}

	EffectThread(const EffectThread &)            = delete;

	EffectThread &operator=(const EffectThread &) = delete;

	friend bool operator==(const std::unique_ptr<EffectThread> &pThisThread, DWORD64 ullThreadId)
	{
		return pThisThread->m_ullId == ullThreadId;
	}

	friend bool operator==(const std::unique_ptr<EffectThread> &pThisThread, LPVOID pThread)
	{
		return pThisThread->m_pThread == pThread;
	}

	inline void OnRun()
	{
		m_ThreadData.m_CallerFiber = GetCurrentFiber();
		SwitchToFiber(m_pThread);
	}

	inline void Stop()
	{
		if (!m_bHasStopped)
		{
			m_bIsRunning = false;

			OnRun();
		}
	}

	inline bool HasStopped() const
	{
		return m_bHasStopped;
	}

	inline bool HasOnStartExecuted() const
	{
		return m_bHasOnStartExecuted;
	}

	inline bool IsStopping() const
	{
		return !m_bIsRunning;
	}
};
