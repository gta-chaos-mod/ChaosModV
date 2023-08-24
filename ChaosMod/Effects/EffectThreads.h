#pragma once

#include "Effects/Effect.h"

#include "Util/CrashHandler.h"

#include <memory>

using DWORD   = unsigned long;
using DWORD64 = unsigned long long;

using LPVOID  = void *;

namespace EffectThreads
{
	LPVOID CreateThread(RegisteredEffect *effect, bool isTimed);

	void StopThread(LPVOID threadId);
	void StopThreadImmediately(LPVOID threadId);
	void StopThreads();
	void StopThreadsImmediately();

	void PauseThisThread(DWORD timeMs);
	bool IsThreadPaused(LPVOID threadId);

	void RunThreads();
	void RunThread(LPVOID threadId);

	bool DoesThreadExist(LPVOID threadId);
	bool HasThreadOnStartExecuted(LPVOID threadId);

	bool IsThreadAnEffectThread();
};

struct EffectThreadData
{
	RegisteredEffect *Effect = nullptr;
	bool *HasOnStartExecuted = nullptr;
	bool *IsRunning          = nullptr;
	bool *HasStopped         = nullptr;

	void *CallerFiber        = nullptr;

	EffectThreadData(RegisteredEffect *effect, bool *hasOnStartExecuted, bool *isRunning, bool *hasStopped)
	    : Effect(effect), HasOnStartExecuted(hasOnStartExecuted), IsRunning(isRunning), HasStopped(hasStopped)
	{
	}
};

inline void EffectThreadFunc(LPVOID data)
{
	SetUnhandledExceptionFilter(CrashHandler);

	auto &threadData = *reinterpret_cast<EffectThreadData *>(data);

	threadData.Effect->Start();
	*threadData.HasOnStartExecuted = true;

	while (*threadData.IsRunning)
	{
		SwitchToFiber(threadData.CallerFiber);
		threadData.Effect->Tick();
	}

	SwitchToFiber(threadData.CallerFiber);
	threadData.Effect->Stop();

	*threadData.HasStopped = true;
	SwitchToFiber(threadData.CallerFiber);
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
	DWORD64 PauseTimestamp = 0;
	LPVOID Thread          = nullptr;

	EffectThread(RegisteredEffect *effect, bool isTimed)
	    : m_Effect(effect),
	      m_IsRunning(isTimed),
	      m_ThreadData(effect, &m_HasOnStartExecuted, &m_IsRunning, &m_HasStopped),
	      Thread(CreateFiber(0, EffectThreadFunc, &m_ThreadData))
	{
	}

	~EffectThread()
	{
		DeleteFiber(Thread);
	}

	EffectThread(const EffectThread &)            = delete;

	EffectThread &operator=(const EffectThread &) = delete;

	friend bool operator==(const std::unique_ptr<EffectThread> &thisThread, LPVOID thread)
	{
		return thisThread->Thread == thread;
	}

	inline void OnRun()
	{
		m_ThreadData.CallerFiber = GetCurrentFiber();
		SwitchToFiber(Thread);
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