#pragma once

#include "Effects/Effect.h"

#include "Util/CrashHandler.h"

#include <list>
#include <memory>

typedef unsigned long DWORD;
typedef unsigned long long DWORD64;

struct EffectThreadData
{
public:
	EffectThreadData(RegisteredEffect* effect, bool& hasOnStartExecuted, bool& isRunning, bool& hasStopped) : Effect(effect), HasOnStartExecuted(hasOnStartExecuted),
		IsRunning(isRunning), HasStopped(hasStopped)
	{

	}
	
public:
	RegisteredEffect* Effect;
	bool& IsRunning;
	bool& HasOnStartExecuted;
	bool& HasStopped;
};

namespace ThreadManager
{
	DWORD64 CreateThread(RegisteredEffect* effect, bool isTimed);
	void StopThread(DWORD64 threadId);
	void StopThreads();
	void PutThreadOnPause(DWORD ms);
	void RunThreads();
	void SwitchToMainThread();
	bool HasThreadOnStartExecuted(DWORD64 threadId);
	bool IsAnyThreadRunningOnStart();
	bool IsAnyThreadRunning();
	void ClearThreads();
};

static void EffectThreadFunc(void* data)
{
	SetUnhandledExceptionFilter(CrashHandler);

	extern void WAIT(DWORD ms);

	EffectThreadData threadData = *reinterpret_cast<EffectThreadData*>(data);

	threadData.Effect->Start();

	threadData.HasOnStartExecuted = true;

	while (threadData.IsRunning)
	{
		threadData.Effect->Tick();

		WAIT(0);
	}

	threadData.Effect->Stop();

	threadData.HasStopped = true;

	ThreadManager::SwitchToMainThread();
}

struct EffectThread
{
public:
	EffectThread(RegisteredEffect* effect, bool isTimed) : m_effect(effect), m_isRunning(isTimed), m_threadData(effect, m_hasOnStartExecuted, m_isRunning, m_hasStopped),
		Thread(CreateFiber(0, EffectThreadFunc, &m_threadData)), Id(m_lastId++)
	{
		
	}

	~EffectThread()
	{
		DeleteFiber(Thread);
	}

	friend bool operator==(const std::unique_ptr<EffectThread>& thisThread, DWORD64 threadId)
	{
		return thisThread->Id == threadId;
	}

	friend bool operator==(const std::unique_ptr<EffectThread>& thisThread, void* thread)
	{
		return thisThread->Thread == thread;
	}

public:
	int PauseTime = 0;
	void* const Thread;
	const DWORD64 Id = 0;

	void Run() const
	{
		SwitchToFiber(Thread);
	}

	void Stop()
	{
		if (!m_hasStopped)
		{
			m_isRunning = false;

			Run();
		}
	}

	bool HasStopped() const
	{
		return m_hasStopped;
	}

	bool HasOnStartExecuted() const
	{
		return m_hasOnStartExecuted;
	}

private:
	static inline DWORD64 m_lastId = 0;

	RegisteredEffect* m_effect;
	bool m_hasOnStartExecuted = false;
	bool m_isRunning;
	bool m_hasStopped = false;
	EffectThreadData m_threadData;
};