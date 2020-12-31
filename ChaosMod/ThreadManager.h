#pragma once

#include "Effects/Effect.h"

#include <list>
#include <memory>

typedef unsigned long DWORD;
typedef unsigned long long DWORD64;

struct EffectThreadData
{
public:
	EffectThreadData(RegisteredEffect* effect, std::string* name, bool& isRunning, bool& hasStopped) : Effect(effect), Name(name), IsRunning(isRunning), HasStopped(hasStopped)
	{

	}
	
public:
	RegisteredEffect* Effect;
	std::string* Name;
	bool& IsRunning;
	bool& HasStopped;
};

namespace ThreadManager
{
	DWORD64 CreateThread(RegisteredEffect* effect, bool isTimed, std::string* = nullptr);
	void StopThread(DWORD64 threadId);
	void StopThreads();
	void PutThreadOnPause(DWORD ms);
	void RunThreads();
	void SwitchToMainThread();
};

static void EffectThreadFunc(void* data)
{
	extern void WAIT(DWORD ms);

	EffectThreadData threadData = *reinterpret_cast<EffectThreadData*>(data);

	std::string origName = threadData.Name ? *threadData.Name : "";

	if (!origName.empty())
	{
		*threadData.Name = "";
	}

	threadData.Effect->Start();

	if (threadData.Name && !origName.empty())
	{
		*threadData.Name = origName;
	}

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
	EffectThread(RegisteredEffect* effect, bool isTimed, std::string* name = nullptr) : m_effect(effect), m_isRunning(isTimed), m_threadData(effect, name, m_isRunning, m_hasStopped),
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

private:
	static inline DWORD64 m_lastId = 0;

	RegisteredEffect* m_effect;
	bool m_isRunning;
	bool m_hasStopped = false;
	EffectThreadData m_threadData;
};